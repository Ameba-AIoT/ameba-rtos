/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*------------------------------------------------------------------------------

    Table of contents

    1. Include headers
    2. External compiler flags
    3. Module defines
    4. Local function prototypes
    5. Functions
        - JpegDecClearStructs
        - JpegDecInitHW
        - JpegDecAllocateResidual
        - JpegDecWriteTables
        - JpegRefreshRegs
        - JpegFlushRegs

------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "basetype.h"
#include "jpegdeccontainer.h"
#include "jpegdecapi.h"
#include "jpegdecmarkers.h"
#include "jpegdecutils.h"
#include "jpegdechdrs.h"
#include "jpegdecscan.h"
#include "jpegregdrv.h"
#include "jpegdecinternal.h"
#include "dwl.h"
#include "deccfg.h"

#ifdef JPEGDEC_ASIC_TRACE
#include "jpegasicdbgtrace.h"
#endif /* #ifdef JPEGDEC_TRACE */

#ifdef JPEGDEC_PP_TRACE
#include "ppinternal.h"
#endif /* #ifdef JPEGDEC_PP_TRACE */

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

static const u8 zzOrder[64] = {
	0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5,
	12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

#define JPEGDEC_SLICE_START_VALUE 0
#define JPEGDEC_VLC_LEN_START_REG 16
#define JPEGDEC_VLC_LEN_END_REG 29

#ifdef PJPEG_COMPONENT_TRACE
extern u32 pjpegComponentId;
extern u32 *pjpegCoeffBase;
extern u32 pjpegCoeffSize;

#define TRACE_COMPONENT_ID(id) pjpegComponentId = id
#else
#define TRACE_COMPONENT_ID(id)
#endif

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/
static void JpegDecWriteTables(JpegDecContainer *pJpegDecCont);
static void JpegDecWriteTablesNonInterleaved(JpegDecContainer *pJpegDecCont);
static void JpegDecWriteTablesProgressive(JpegDecContainer *pJpegDecCont);
static void JpegDecChromaTableSelectors(JpegDecContainer *pJpegDecCont);
static void JpegDecSetHwStrmParams(JpegDecContainer *pJpegDecCont);
static void JpegDecWriteLenBits(JpegDecContainer *pJpegDecCont);
static void JpegDecWriteLenBitsNonInterleaved(JpegDecContainer *pJpegDecCont);
static void JpegDecWriteLenBitsProgressive(JpegDecContainer *pJpegDecCont);

/*------------------------------------------------------------------------------
    5. Functions
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------

        Function name: JpegDecClearStructs

        Functional description:
          handles the initialisation of jpeg decoder data structure

        Inputs:

        Outputs:
          Returns OK when successful, NOK in case unknown message type is
          asked

------------------------------------------------------------------------------*/
void JpegDecClearStructs(JpegDecContainer *pJpegDecCont, u32 mode)
{
	u32 i;

	ASSERT(pJpegDecCont);

	/* stream pointers */
	pJpegDecCont->stream.streamBus = 0;
	pJpegDecCont->stream.pStartOfStream = NULL;
	pJpegDecCont->stream.pCurrPos = NULL;
	pJpegDecCont->stream.bitPosInByte = 0;
	pJpegDecCont->stream.streamLength = 0;
	pJpegDecCont->stream.readBits = 0;
	pJpegDecCont->stream.appnFlag = 0;
	pJpegDecCont->stream.returnSosMarker = 0;

	/* output image pointers and variables */
	pJpegDecCont->image.pStartOfImage = NULL;
	pJpegDecCont->image.pLum = NULL;
	pJpegDecCont->image.pCr = NULL;
	pJpegDecCont->image.pCb = NULL;
	pJpegDecCont->image.headerReady = 0;
	pJpegDecCont->image.imageReady = 0;
	pJpegDecCont->image.ready = 0;
	pJpegDecCont->image.size = 0;
	pJpegDecCont->image.sizeLuma = 0;
	pJpegDecCont->image.sizeChroma = 0;
	for (i = 0; i < MAX_NUMBER_OF_COMPONENTS; i++) {
		pJpegDecCont->image.columns[i] = 0;
		pJpegDecCont->image.pixelsPerRow[i] = 0;
	}

	/* frame info */
	pJpegDecCont->frame.Lf = 0;
	pJpegDecCont->frame.P = 0;
	pJpegDecCont->frame.Y = 0;
	pJpegDecCont->frame.X = 0;

	if (!mode) {
		pJpegDecCont->frame.hwY = 0;
		pJpegDecCont->frame.hwX = 0;
		pJpegDecCont->frame.hwYTn = 0;
		pJpegDecCont->frame.hwXTn = 0;
		pJpegDecCont->frame.fullX = 0;
		pJpegDecCont->frame.fullY = 0;
		pJpegDecCont->stream.thumbnail = 0;
	} else {
		if (pJpegDecCont->stream.thumbnail) {
			pJpegDecCont->frame.hwY = pJpegDecCont->frame.fullY;
			pJpegDecCont->frame.hwX = pJpegDecCont->frame.fullX;
			pJpegDecCont->stream.thumbnail = 0;
		}
	}

	pJpegDecCont->frame.Nf = 0; /* Number of components in frame */
	pJpegDecCont->frame.codingType = 0;
	pJpegDecCont->frame.numMcuInFrame = 0;
	pJpegDecCont->frame.numMcuInRow = 0;
	pJpegDecCont->frame.mcuNumber = 0;
	pJpegDecCont->frame.Ri = 0;
	pJpegDecCont->frame.row = 0;
	pJpegDecCont->frame.col = 0;
	pJpegDecCont->frame.driPeriod = 0;
	pJpegDecCont->frame.block = 0;
	pJpegDecCont->frame.cIndex = 0;
	pJpegDecCont->frame.bufferBus = 0;

	if (!mode) {
		pJpegDecCont->frame.pBuffer = NULL;
		pJpegDecCont->frame.pBufferCb = NULL;
		pJpegDecCont->frame.pBufferCr = NULL;
		pJpegDecCont->frame.pTableBase.virtualAddress = NULL;
		pJpegDecCont->frame.pTableBase.busAddress = 0;

		/* asic buffer */
		pJpegDecCont->asicBuff.outLumaBuffer.virtualAddress = NULL;
		pJpegDecCont->asicBuff.outChromaBuffer.virtualAddress = NULL;
		pJpegDecCont->asicBuff.outChromaBuffer2.virtualAddress = NULL;
		pJpegDecCont->asicBuff.outLumaBuffer.busAddress = 0;
		pJpegDecCont->asicBuff.outChromaBuffer.busAddress = 0;
		pJpegDecCont->asicBuff.outChromaBuffer2.busAddress = 0;
		pJpegDecCont->asicBuff.outLumaBuffer.size = 0;
		pJpegDecCont->asicBuff.outChromaBuffer.size = 0;
		pJpegDecCont->asicBuff.outChromaBuffer2.size = 0;

		/* pp instance */
		pJpegDecCont->ppStatus = 0;
		pJpegDecCont->ppInstance = NULL;
		pJpegDecCont->PPRun = NULL;
		pJpegDecCont->PPEndCallback = NULL;
		pJpegDecCont->ppControl.usePipeline = 0;

		/* resolution */
		pJpegDecCont->minSupportedWidth = 0;
		pJpegDecCont->minSupportedHeight = 0;
		pJpegDecCont->maxSupportedWidth = 0;
		pJpegDecCont->maxSupportedHeight = 0;
		pJpegDecCont->maxSupportedPixelAmount = 0;
		pJpegDecCont->maxSupportedSliceSize = 0;

		/* out bus tmp */
		pJpegDecCont->info.outLuma.virtualAddress = NULL;
		pJpegDecCont->info.outChroma.virtualAddress = NULL;
		pJpegDecCont->info.outChroma2.virtualAddress = NULL;

		/* user allocated addresses */
		pJpegDecCont->info.givenOutLuma.virtualAddress = NULL;
		pJpegDecCont->info.givenOutChroma.virtualAddress = NULL;
		pJpegDecCont->info.givenOutChroma2.virtualAddress = NULL;
	}

	/* asic running flag */
	pJpegDecCont->asicRunning = 0;

	/* image handling info */
	pJpegDecCont->info.sliceHeight = 0;
	pJpegDecCont->info.amountOfQTables = 0;
	pJpegDecCont->info.yCbCrMode = 0;
	pJpegDecCont->info.yCbCr422 = 0;
	pJpegDecCont->info.column = 0;
	pJpegDecCont->info.X = 0;
	pJpegDecCont->info.Y = 0;
	pJpegDecCont->info.memSize = 0;
	pJpegDecCont->info.SliceCount = 0;
	pJpegDecCont->info.SliceMBCutValue = 0;
	pJpegDecCont->info.pipeline = 0;
	if (!mode) {
		pJpegDecCont->info.userAllocMem = 0;
	}
	pJpegDecCont->info.sliceStartCount = 0;
	pJpegDecCont->info.amountOfSlices = 0;
	pJpegDecCont->info.noSliceIrqForUser = 0;
	pJpegDecCont->info.SliceReadyForPause = 0;
	pJpegDecCont->info.sliceLimitReached = 0;
	pJpegDecCont->info.sliceMbSetValue = 0;
	pJpegDecCont->info.timeout = (u32) DEC_X170_TIMEOUT_LENGTH;
	pJpegDecCont->info.rlcMode = 0; /* JPEG always in VLC mode == 0 */
	pJpegDecCont->info.lumaPos = 0;
	pJpegDecCont->info.chromaPos = 0;
	pJpegDecCont->info.fillRight = 0;
	pJpegDecCont->info.fillBottom = 0;
	pJpegDecCont->info.streamEnd = 0;
	pJpegDecCont->info.streamEndFlag = 0;
	pJpegDecCont->info.inputBufferEmpty = 0;
	pJpegDecCont->info.inputStreaming = 0;
	pJpegDecCont->info.inputBufferLen = 0;
	pJpegDecCont->info.decodedStreamLen = 0;
	pJpegDecCont->info.init = 0;
	pJpegDecCont->info.initThumb = 0;
	pJpegDecCont->info.initBufferSize = 0;

	/* progressive */
	pJpegDecCont->info.nonInterleaved = 0;
	pJpegDecCont->info.componentId = 0;
	pJpegDecCont->info.operationType = 0;
	pJpegDecCont->info.operationTypeThumb = 0;
	pJpegDecCont->info.progressiveScanReady = 0;
	pJpegDecCont->info.nonInterleavedScanReady = 0;

	if (!mode) {
		pJpegDecCont->info.pCoeffBase.virtualAddress = NULL;
		pJpegDecCont->info.pCoeffBase.busAddress = 0;
		pJpegDecCont->info.yCbCrModeOrig = 0;
		pJpegDecCont->info.getInfoYCbCrMode = 0;
		pJpegDecCont->info.getInfoYCbCrModeTn = 0;
	}

	pJpegDecCont->info.allocated = 0;
	pJpegDecCont->info.progressiveFinish = 0;
	pJpegDecCont->info.pfCompId = 0;
	for (i = 0; i < MAX_NUMBER_OF_COMPONENTS; i++) {
		pJpegDecCont->info.pfNeeded[i] = 0;
	}
	pJpegDecCont->info.tmpStrm.virtualAddress = NULL;

	for (i = 0; i < MAX_NUMBER_OF_COMPONENTS; i++) {
		pJpegDecCont->info.components[i] = 0;
		pJpegDecCont->info.pred[i] = 0;
		pJpegDecCont->info.dcRes[i] = 0;
		pJpegDecCont->frame.numBlocks[i] = 0;
		pJpegDecCont->frame.blocksPerRow[i] = 0;
		pJpegDecCont->frame.useAcOffset[i] = 0;
		pJpegDecCont->frame.component[i].C = 0;
		pJpegDecCont->frame.component[i].H = 0;
		pJpegDecCont->frame.component[i].V = 0;
		pJpegDecCont->frame.component[i].Tq = 0;
	}

	/* scan info */
	pJpegDecCont->scan.Ls = 0;
	pJpegDecCont->scan.Ns = 0;
	pJpegDecCont->scan.Ss = 0;
	pJpegDecCont->scan.Se = 0;
	pJpegDecCont->scan.Ah = 0;
	pJpegDecCont->scan.Al = 0;
	pJpegDecCont->scan.index = 0;
	pJpegDecCont->scan.numIdctRows = 0;

	for (i = 0; i < MAX_NUMBER_OF_COMPONENTS; i++) {
		pJpegDecCont->scan.Cs[i] = 0;
		pJpegDecCont->scan.Td[i] = 0;
		pJpegDecCont->scan.Ta[i] = 0;
		pJpegDecCont->scan.pred[i] = 0;
	}

	/* huffman table lengths */
	pJpegDecCont->vlc.defaultTables = 0;
	pJpegDecCont->vlc.acTable0.tableLength = 0;
	pJpegDecCont->vlc.acTable1.tableLength = 0;
	pJpegDecCont->vlc.acTable2.tableLength = 0;
	pJpegDecCont->vlc.acTable3.tableLength = 0;

	pJpegDecCont->vlc.dcTable0.tableLength = 0;
	pJpegDecCont->vlc.dcTable1.tableLength = 0;
	pJpegDecCont->vlc.dcTable2.tableLength = 0;
	pJpegDecCont->vlc.dcTable3.tableLength = 0;

	/* Restart interval */
	pJpegDecCont->frame.Ri = 0;

	if (pJpegDecCont->vlc.acTable0.vals) {
		DWLfree(pJpegDecCont->vlc.acTable0.vals);
	}
	if (pJpegDecCont->vlc.acTable1.vals) {
		DWLfree(pJpegDecCont->vlc.acTable1.vals);
	}
	if (pJpegDecCont->vlc.acTable2.vals) {
		DWLfree(pJpegDecCont->vlc.acTable2.vals);
	}
	if (pJpegDecCont->vlc.acTable3.vals) {
		DWLfree(pJpegDecCont->vlc.acTable3.vals);
	}
	if (pJpegDecCont->vlc.dcTable0.vals) {
		DWLfree(pJpegDecCont->vlc.dcTable0.vals);
	}
	if (pJpegDecCont->vlc.dcTable1.vals) {
		DWLfree(pJpegDecCont->vlc.dcTable1.vals);
	}
	if (pJpegDecCont->vlc.dcTable2.vals) {
		DWLfree(pJpegDecCont->vlc.dcTable2.vals);
	}
	if (pJpegDecCont->vlc.dcTable3.vals) {
		DWLfree(pJpegDecCont->vlc.dcTable3.vals);
	}
	if (pJpegDecCont->frame.pBuffer) {
		DWLfree(pJpegDecCont->frame.pBuffer);
	}

	/* pointer initialisation */
	pJpegDecCont->vlc.acTable0.vals = NULL;
	pJpegDecCont->vlc.acTable1.vals = NULL;
	pJpegDecCont->vlc.acTable2.vals = NULL;
	pJpegDecCont->vlc.acTable3.vals = NULL;

	pJpegDecCont->vlc.dcTable0.vals = NULL;
	pJpegDecCont->vlc.dcTable1.vals = NULL;
	pJpegDecCont->vlc.dcTable2.vals = NULL;
	pJpegDecCont->vlc.dcTable3.vals = NULL;

	pJpegDecCont->frame.pBuffer = NULL;

	return;
}
/*------------------------------------------------------------------------------

        Function name: JpegDecInitHW

        Functional description:
          Set up HW regs for decode

        Inputs:
          JpegDecContainer *pJpegDecCont

        Outputs:
          Returns OK when successful, NOK in case unknown message type is
          asked

------------------------------------------------------------------------------*/
JpegDecRet JpegDecInitHW(JpegDecContainer *pJpegDecCont)
{
	u32 i;
	g1_addr_t coeffBuffer = 0;

#define PTR_JPGC   pJpegDecCont

	ASSERT(pJpegDecCont);

	TRACE_COMPONENT_ID(PTR_JPGC->info.componentId);

	/* Check if first InitHw call */
	if (PTR_JPGC->info.sliceStartCount == 0) {
		/* Check if HW resource is available */
		if (DWLReserveHw(pJpegDecCont->dwl, &pJpegDecCont->coreID) == DWL_ERROR) {
			JPEGDEC_TRACE_INTERNAL(("JpegDecInitHW: ERROR hw resource unavailable"));
			return JPEGDEC_HW_RESERVED;
		}
	}

	/*************** Set swreg4 data ************/
	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame width extension\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PIC_MB_W_EXT,
				   ((((PTR_JPGC->info.X) >> (4)) & 0xE00) >> 9));

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame width\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PIC_MB_WIDTH,
				   ((PTR_JPGC->info.X) >> (4)) & 0x1FF);

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame height extension\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PIC_MB_H_EXT,
				   ((((PTR_JPGC->info.Y) >> (4)) & 0x700) >> 8));

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame height\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PIC_MB_HEIGHT_P,
				   ((PTR_JPGC->info.Y) >> (4)) & 0x0FF);

	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set decoding mode: JPEG\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_MODE, JPEG_X170_MODE_JPEG);

	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set output write enabled\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_DIS, 0);

	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set filtering disabled\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_FILTERING_DIS, 1);

	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set amount of QP Table\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_JPEG_QTABLES,
				   PTR_JPGC->info.amountOfQTables);

	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set input format\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_JPEG_MODE,
				   PTR_JPGC->info.yCbCrMode);

	JPEGDEC_TRACE_INTERNAL(("INTERNAL: RLC mode enable, JPEG == disable\n"));
	/* In case of JPEG: Always VLC mode used (0) */
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_RLC_MODE_E, PTR_JPGC->info.rlcMode);

	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Width is not multiple of 16\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_JPEG_FILRIGHT_E,
				   PTR_JPGC->info.fillRight);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_FILDOWN_E,
				   PTR_JPGC->info.fillBottom);

	/*************** Set swreg15 data ************/
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set slice/full mode: 0 full; other = slice\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_JPEG_SLICE_H,
				   PTR_JPGC->info.sliceHeight);

	/*************** Set swreg52 data ************/
	if (PTR_JPGC->info.operationType != JPEGDEC_PROGRESSIVE) {
		/* Set JPEG operation mode */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_E, 0);
	} else {
		/* Set JPEG operation mode */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_E, 1);
	}

	/* Set spectral selection start coefficient */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_SS, PTR_JPGC->scan.Ss);

	/* Set spectral selection end coefficient */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_SE, PTR_JPGC->scan.Se);

	/* Set the point transform used in the preceding scan */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_AH, PTR_JPGC->scan.Ah);

	/* Set the point transform value */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_AL, PTR_JPGC->scan.Al);

	/* Set needed progressive parameters */
	if (PTR_JPGC->info.operationType == JPEGDEC_PROGRESSIVE) {
		/* write coeff table base */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set coefficient buffer base address\n"));
		coeffBuffer = PTR_JPGC->info.pCoeffBase.busAddress;
		/* non-interleaved */
		if (PTR_JPGC->info.nonInterleaved) {
			for (i = 0; i < PTR_JPGC->info.componentId; i++) {
				coeffBuffer += (JPEGDEC_COEFF_SIZE *
								PTR_JPGC->frame.numBlocks[i]);
			}
			SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_PJPEG_COEFF_BUF,
						 coeffBuffer);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_DIS, 0);
		}
		/* interleaved components */
		else {
			SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_PJPEG_COEFF_BUF,
						 coeffBuffer);
			coeffBuffer += (JPEGDEC_COEFF_SIZE) * PTR_JPGC->frame.numBlocks[0];
			SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_PJPEG_DCCB_BASE,
						 coeffBuffer);
			coeffBuffer += (JPEGDEC_COEFF_SIZE) * PTR_JPGC->frame.numBlocks[1];
			SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_PJPEG_DCCR_BASE,
						 coeffBuffer);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_DIS, 1);
		}
	}

	/*************** Set swreg5/swreg6/swreg12/swreg16-swreg27 data ************/

	if (PTR_JPGC->info.operationType == JPEGDEC_BASELINE) {
		/* write "length amounts" */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write VLC length amounts to register\n"));
		JpegDecWriteLenBits(PTR_JPGC);

		/* Create AC/DC/QP tables for HW */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write AC,DC,QP tables to base\n"));
		JpegDecWriteTables(PTR_JPGC);

	} else if (PTR_JPGC->info.operationType == JPEGDEC_NONINTERLEAVED) {
		/* write "length amounts" */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write VLC length amounts to register\n"));
		JpegDecWriteLenBitsNonInterleaved(PTR_JPGC);

		/* Create AC/DC/QP tables for HW */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write AC,DC,QP tables to base\n"));
		JpegDecWriteTablesNonInterleaved(PTR_JPGC);
	} else {
		/* write "length amounts" */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write VLC length amounts to register\n"));
		JpegDecWriteLenBitsProgressive(PTR_JPGC);

		/* Create AC/DC/QP tables for HW */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write AC,DC,QP tables to base\n"));
		JpegDecWriteTablesProgressive(PTR_JPGC);
	}

	/* Select which tables the chromas use */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Select chroma AC,DC tables\n"));
	JpegDecChromaTableSelectors(PTR_JPGC);

	/* write table base */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set AC,DC,QP table base address\n"));
	SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_QTABLE_BASE,
				 PTR_JPGC->frame.pTableBase.busAddress);
	DCache_Clean(PTR_JPGC->frame.pTableBase.busAddress, PTR_JPGC->frame.pTableBase.size);

	/* set up stream position for HW decode */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set stream position for HW\n"));
	JpegDecSetHwStrmParams(PTR_JPGC);

	/* set restart interval */
	if (PTR_JPGC->frame.Ri) {
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_SYNC_MARKER_E, 1);
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_REST_FREQ,
					   PTR_JPGC->frame.Ri);
	} else {
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_SYNC_MARKER_E, 0);
	}

	/* Handle PP and output base addresses */

	/* PP depending register writes */
	if (PTR_JPGC->ppInstance != NULL && PTR_JPGC->ppControl.usePipeline) {
		/*************** Set swreg4 data ************/

		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set output write disabled\n"));
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_DIS, 1);

		/* set output to zero, because of pp */
		/*************** Set swreg13 data ************/
		/* Luminance output */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set LUMA OUTPUT data base address\n"));
		SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_BASE, (g1_addr_t)0);

		/*************** Set swreg14 data ************/
		/* Chrominance output */
		if (PTR_JPGC->image.sizeChroma) {
			/* write output base */
			JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set CHROMA OUTPUT data base address\n"));
			SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_JPG_CH_OUT_BASE, (g1_addr_t)0);
		}

		if (PTR_JPGC->info.sliceStartCount == JPEGDEC_SLICE_START_VALUE) {
			/* Enable pp */
			JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Enable pp\n"));
			PTR_JPGC->PPRun(PTR_JPGC->ppInstance, &PTR_JPGC->ppControl);

			PTR_JPGC->ppControl.ppStatus = DECPP_RUNNING;
		}

		PTR_JPGC->info.pipeline = 1;
	} else {
		/*************** Set swreg13 data ************/

		/* Luminance output */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set LUMA OUTPUT data base address\n"));

		if (PTR_JPGC->info.operationType == JPEGDEC_BASELINE) {
			SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_BASE,
						 PTR_JPGC->asicBuff.outLumaBuffer.busAddress);

			/*************** Set swreg14 data ************/

			/* Chrominance output */
			if (PTR_JPGC->image.sizeChroma) {
				/* write output base */
				JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set CHROMA OUTPUT data base address\n"));
				SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_JPG_CH_OUT_BASE,
							 PTR_JPGC->asicBuff.outChromaBuffer.busAddress);
			}
		} else {
			if (PTR_JPGC->info.componentId == 0) {
				SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_BASE,
							 PTR_JPGC->asicBuff.outLumaBuffer.busAddress);
			} else if (PTR_JPGC->info.componentId == 1) {
				SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_BASE,
							 PTR_JPGC->asicBuff.outChromaBuffer.busAddress);
			} else {
				SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_BASE,
							 (PTR_JPGC->asicBuff.outChromaBuffer2.
							  busAddress));
			}
		}

		PTR_JPGC->info.pipeline = 0;
	}

	PTR_JPGC->info.sliceStartCount = 1;

#ifdef JPEGDEC_ASIC_TRACE
	{
		FILE *fd;

		fd = fopen("picture_ctrl_dec.trc", "at");
		DumpJPEGCtrlReg(pJpegDecCont->jpegRegs, fd);
		fclose(fd);

		fd = fopen("picture_ctrl_dec.hex", "at");
		HexDumpJPEGCtrlReg(pJpegDecCont->jpegRegs, fd);
		fclose(fd);

		fd = fopen("jpeg_tables.hex", "at");
		HexDumpJPEGTables(pJpegDecCont->jpegRegs, pJpegDecCont, fd);
		fclose(fd);

		fd = fopen("registers.hex", "at");
		HexDumpRegs(pJpegDecCont->jpegRegs, fd);
		fclose(fd);
	}
#endif /* #ifdef JPEGDEC_ASIC_TRACE */

#ifdef JPEGDEC_PP_TRACE
	ppRegDump(((PPContainer_t *) PTR_JPGC->ppInstance)->ppRegs);
#endif /* #ifdef JPEGDEC_PP_TRACE */

	PTR_JPGC->asicRunning = 1;

	/* Flush regs to hw register */
	JpegFlushRegs(PTR_JPGC);
	DWLWriteReg(pJpegDecCont->dwl, pJpegDecCont->coreID, 4 * 101, PTR_JPGC->jpegRegs[101]);

	/* Enable jpeg mode and set slice mode */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Enable jpeg\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_E, 1);
	DWLEnableHW(PTR_JPGC->dwl, PTR_JPGC->coreID, 4 * 1, PTR_JPGC->jpegRegs[1]);

#undef PTR_JPGC

	return JPEGDEC_OK;
}

/*------------------------------------------------------------------------------

        Function name: JpegDecInitHWContinue

        Functional description:
          Set up HW regs for decode

        Inputs:
          JpegDecContainer *pJpegDecCont

        Outputs:
          Returns OK when successful, NOK in case unknown message type is
          asked

------------------------------------------------------------------------------*/
void JpegDecInitHWContinue(JpegDecContainer *pJpegDecCont)
{
#define PTR_JPGC   pJpegDecCont

	ASSERT(pJpegDecCont);

	/* update slice counter */
	PTR_JPGC->info.amountOfSlices++;

	if (PTR_JPGC->ppInstance == NULL &&
		PTR_JPGC->info.userAllocMem == 1 && PTR_JPGC->info.sliceStartCount > 0) {
		/* if user allocated memory ==> new addresses */
		PTR_JPGC->asicBuff.outLumaBuffer.virtualAddress =
			PTR_JPGC->info.givenOutLuma.virtualAddress;
		PTR_JPGC->asicBuff.outLumaBuffer.busAddress =
			PTR_JPGC->info.givenOutLuma.busAddress;
		PTR_JPGC->asicBuff.outChromaBuffer.virtualAddress =
			PTR_JPGC->info.givenOutChroma.virtualAddress;
		PTR_JPGC->asicBuff.outChromaBuffer.busAddress =
			PTR_JPGC->info.givenOutChroma.busAddress;
	}

	/* Update only register/values that might have been changed */

	/*************** Set swreg1 data ************/
	/* clear status bit */
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_SLICE_INT, 0);

	/*************** Set swreg5 data ************/
	JPEGDEC_TRACE_INTERNAL(("INTERNAL CONTINUE: Set stream last buffer bit\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_JPEG_STREAM_ALL,
				   PTR_JPGC->info.streamEnd);

	/*************** Set swreg13 data ************/
	/* PP depending register writes */
	if (PTR_JPGC->ppInstance == NULL) {
		/* Luminance output */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL CONTINUE: Set LUMA OUTPUT data base address\n"));
		SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_BASE,
					 PTR_JPGC->asicBuff.outLumaBuffer.busAddress);

		/*************** Set swreg14 data ************/

		/* Chrominance output */
		if (PTR_JPGC->image.sizeChroma) {
			/* write output base */
			JPEGDEC_TRACE_INTERNAL(("INTERNAL CONTINUE: Set CHROMA OUTPUT data base address\n"));
			SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_JPG_CH_OUT_BASE,
						 PTR_JPGC->asicBuff.outChromaBuffer.busAddress);
		}

		PTR_JPGC->info.pipeline = 0;
	}

	/*************** Set swreg13 data ************/
	/* PP depending register writes */
	if (PTR_JPGC->ppInstance != NULL && PTR_JPGC->ppControl.usePipeline == 0) {
		if (PTR_JPGC->info.yCbCrMode == JPEGDEC_YUV420) {
			PTR_JPGC->info.lumaPos = (PTR_JPGC->info.X *
									  (PTR_JPGC->info.sliceMbSetValue * 16));
			PTR_JPGC->info.chromaPos = ((PTR_JPGC->info.X) *
										(PTR_JPGC->info.sliceMbSetValue * 8));
		} else if (PTR_JPGC->info.yCbCrMode == JPEGDEC_YUV422) {
			PTR_JPGC->info.lumaPos = (PTR_JPGC->info.X *
									  (PTR_JPGC->info.sliceMbSetValue * 16));
			PTR_JPGC->info.chromaPos = ((PTR_JPGC->info.X) *
										(PTR_JPGC->info.sliceMbSetValue * 16));
		} else if (PTR_JPGC->info.yCbCrMode == JPEGDEC_YUV440) {
			PTR_JPGC->info.lumaPos = (PTR_JPGC->info.X *
									  (PTR_JPGC->info.sliceMbSetValue * 16));
			PTR_JPGC->info.chromaPos = ((PTR_JPGC->info.X) *
										(PTR_JPGC->info.sliceMbSetValue * 16));
		} else {
			PTR_JPGC->info.lumaPos = (PTR_JPGC->info.X *
									  (PTR_JPGC->info.sliceMbSetValue * 16));
			PTR_JPGC->info.chromaPos = 0;
		}

		/* update luma/chroma position */
		PTR_JPGC->info.lumaPos = (PTR_JPGC->info.lumaPos *
								  PTR_JPGC->info.amountOfSlices);
		if (PTR_JPGC->info.chromaPos) {
			PTR_JPGC->info.chromaPos = (PTR_JPGC->info.chromaPos *
										PTR_JPGC->info.amountOfSlices);
		}

		/* Luminance output */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL CONTINUE: Set LUMA OUTPUT data base address\n"));
		SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_BASE,
					 PTR_JPGC->asicBuff.outLumaBuffer.busAddress +
					 PTR_JPGC->info.lumaPos);

		/*************** Set swreg14 data ************/

		/* Chrominance output */
		if (PTR_JPGC->image.sizeChroma) {
			/* write output base */
			JPEGDEC_TRACE_INTERNAL(("INTERNAL CONTINUE: Set CHROMA OUTPUT data base address\n"));
			SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_JPG_CH_OUT_BASE,
						 PTR_JPGC->asicBuff.outChromaBuffer.busAddress +
						 PTR_JPGC->info.chromaPos);
		}

		PTR_JPGC->info.pipeline = 0;
	}

	/*************** Set swreg15 data ************/
	JPEGDEC_TRACE_INTERNAL(("INTERNAL CONTINUE: Set slice/full mode: 0 full; other = slice\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_JPEG_SLICE_H,
				   PTR_JPGC->info.sliceHeight);

	/* Flush regs to hw register */
	DWLWriteReg(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x34,
				PTR_JPGC->jpegRegs[13]);
	DWLWriteReg(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x38,
				PTR_JPGC->jpegRegs[14]);
	DWLWriteReg(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x3C,
				PTR_JPGC->jpegRegs[15]);
#ifdef USE_64BIT_ENV
	DWLWriteReg(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x1EC,
				PTR_JPGC->jpegRegs[123]);
	DWLWriteReg(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x1F0,
				PTR_JPGC->jpegRegs[124]);
	DWLWriteReg(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x1F4,
				PTR_JPGC->jpegRegs[125]);
#endif
	DWLWriteReg(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x14,
				PTR_JPGC->jpegRegs[5]);
	DWLEnableHW(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x4,
				PTR_JPGC->jpegRegs[1]);

#ifdef JPEGDEC_ASIC_TRACE
	{
		JPEGDEC_TRACE_INTERNAL(("INTERNAL CONTINUE: REGS BEFORE IRQ CLEAN\n"));
		PrintJPEGReg(pJpegDecCont->jpegRegs);
	}
#endif /* #ifdef JPEGDEC_ASIC_TRACE */

}

/*------------------------------------------------------------------------------

        Function name: JpegDecInitHWInputBuffLoad

        Functional description:
          Set up HW regs for decode after input buffer load

        Inputs:
          JpegDecContainer *pJpegDecCont

        Outputs:
          Returns OK when successful, NOK in case unknown message type is
          asked

------------------------------------------------------------------------------*/
void JpegDecInitHWInputBuffLoad(JpegDecContainer *pJpegDecCont)
{
#define PTR_JPGC   pJpegDecCont

	ASSERT(pJpegDecCont);

	/* Update only register/values that might have been changed */
	/*************** Set swreg4 data ************/
	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame width extension\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PIC_MB_W_EXT,
				   ((((PTR_JPGC->info.X) >> (4)) & 0xE00) >> 9));

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame width\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PIC_MB_WIDTH,
				   ((PTR_JPGC->info.X) >> (4)) & 0x1FF);

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame height extension\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PIC_MB_H_EXT,
				   ((((PTR_JPGC->info.Y) >> (4)) & 0x700) >> 8));

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame height\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PIC_MB_HEIGHT_P,
				   ((PTR_JPGC->info.Y) >> (4)) & 0x0FF);

	/*************** Set swreg5 data ************/
	JPEGDEC_TRACE_INTERNAL(("INTERNAL BUFFER LOAD: Set stream start bit\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_STRM_START_BIT,
				   PTR_JPGC->stream.bitPosInByte);

	/*************** Set swreg6 data ************/
	JPEGDEC_TRACE_INTERNAL(("INTERNAL BUFFER LOAD: Set stream length\n"));

	/* check if all stream will processed in this buffer */
	if ((PTR_JPGC->info.decodedStreamLen) >= PTR_JPGC->stream.streamLength) {
		PTR_JPGC->info.streamEnd = 1;
	}

	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_STREAM_LEN,
				   PTR_JPGC->info.inputBufferLen);

	/*************** Set swreg4 data ************/
	JPEGDEC_TRACE_INTERNAL(("INTERNAL BUFFER LOAD: Set stream last buffer bit\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_JPEG_STREAM_ALL,
				   PTR_JPGC->info.streamEnd);

	/*************** Set swreg12 data ************/
	JPEGDEC_TRACE_INTERNAL(("INTERNAL BUFFER LOAD: Set stream start address\n"));
	SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_RLC_VLC_BASE,
				 PTR_JPGC->stream.streamBus);

	JPEGDEC_TRACE_INTERNAL(("INTERNAL BUFFER LOAD: Stream bus start 0x%08x\n",
							PTR_JPGC->stream.streamBus));
	JPEGDEC_TRACE_INTERNAL(("INTERNAL BUFFER LOAD: Bit position 0x%08x\n",
							PTR_JPGC->stream.bitPosInByte));
	JPEGDEC_TRACE_INTERNAL(("INTERNAL BUFFER LOAD: Stream length 0x%08x\n",
							PTR_JPGC->stream.streamLength));

	/* Flush regs to hw register */
	DWLWriteReg(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x30,
				PTR_JPGC->jpegRegs[12]);
#ifdef USE_64BIT_ENV
	DWLWriteReg(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x1E8,
				PTR_JPGC->jpegRegs[122]);
#endif
	DWLWriteReg(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x10,
				PTR_JPGC->jpegRegs[4]);
	DWLWriteReg(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x14,
				PTR_JPGC->jpegRegs[5]);
	DWLWriteReg(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x18,
				PTR_JPGC->jpegRegs[6]);
	DWLEnableHW(pJpegDecCont->dwl, PTR_JPGC->coreID, 0x04,
				PTR_JPGC->jpegRegs[1]);

#ifdef JPEGDEC_ASIC_TRACE
	{
		JPEGDEC_TRACE_INTERNAL(("INTERNAL BUFFER LOAD: REGS BEFORE IRQ CLEAN\n"));
		PrintJPEGReg(pJpegDecCont->jpegRegs);
	}
#endif /* #ifdef JPEGDEC_ASIC_TRACE */

}

/*------------------------------------------------------------------------------

        Function name: JpegDecInitHWProgressiveContinue

        Functional description:
          Set up HW regs for decode after progressive scan decoded

        Inputs:
          JpegDecContainer *pJpegDecCont

        Outputs:
          Returns OK when successful, NOK in case unknown message type is
          asked

------------------------------------------------------------------------------*/
void JpegDecInitHWProgressiveContinue(JpegDecContainer *pJpegDecCont)
{
#define PTR_JPGC   pJpegDecCont

	u32 i;
	g1_addr_t coeffBuffer = 0;
	g1_addr_t outputBuffer = 0;

	ASSERT(pJpegDecCont);

	if (PTR_JPGC->ppInstance == NULL && PTR_JPGC->info.userAllocMem == 1) {
		/* if user allocated memory ==> new addresses */
		PTR_JPGC->asicBuff.outLumaBuffer.virtualAddress =
			PTR_JPGC->info.givenOutLuma.virtualAddress;
		PTR_JPGC->asicBuff.outLumaBuffer.busAddress =
			PTR_JPGC->info.givenOutLuma.busAddress;
		PTR_JPGC->asicBuff.outChromaBuffer.virtualAddress =
			PTR_JPGC->info.givenOutChroma.virtualAddress;
		PTR_JPGC->asicBuff.outChromaBuffer.busAddress =
			PTR_JPGC->info.givenOutChroma.busAddress;
		PTR_JPGC->asicBuff.outChromaBuffer2.virtualAddress =
			PTR_JPGC->info.givenOutChroma2.virtualAddress;
		PTR_JPGC->asicBuff.outChromaBuffer2.busAddress =
			PTR_JPGC->info.givenOutChroma2.busAddress;
	}

	TRACE_COMPONENT_ID(PTR_JPGC->info.componentId);
	/* Update only register/values that might have been changed */

	/*************** Set swreg13 data ************/
	/* Luminance output */
	if (PTR_JPGC->info.componentId == 0) {
		outputBuffer = PTR_JPGC->asicBuff.outLumaBuffer.busAddress;
	} else if (PTR_JPGC->info.componentId == 1) {
		outputBuffer = (PTR_JPGC->asicBuff.outChromaBuffer.busAddress);
	} else {
		outputBuffer = (PTR_JPGC->asicBuff.outChromaBuffer2.busAddress);
	}

	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set LUMA OUTPUT data base address\n"));
	SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_BASE, outputBuffer);

	PTR_JPGC->info.pipeline = 0;

	/* set up stream position for HW decode */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set stream position for HW\n"));
	JpegDecSetHwStrmParams(PTR_JPGC);

	/*************** Set swreg5 data ************/
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set input format\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_JPEG_MODE,
				   PTR_JPGC->info.yCbCrMode);

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame width extension\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PIC_MB_W_EXT,
				   ((((PTR_JPGC->info.X) >> (4)) & 0xE00) >> 9));

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame width\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PIC_MB_WIDTH,
				   ((PTR_JPGC->info.X) >> (4)) & 0x1FF);

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame height extension\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PIC_MB_H_EXT,
				   ((((PTR_JPGC->info.Y) >> (4)) & 0x700) >> 8));

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame height\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PIC_MB_HEIGHT_P,
				   ((PTR_JPGC->info.Y) >> (4)) & 0x0FF);

	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_WDIV8, PTR_JPGC->info.fillX);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_JPEG_FILRIGHT_E,
				   PTR_JPGC->info.fillX || PTR_JPGC->info.fillRight);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_HDIV8, PTR_JPGC->info.fillY);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_FILDOWN_E,
				   PTR_JPGC->info.fillY || PTR_JPGC->info.fillBottom);

	/*************** Set swreg52 data ************/
	/* Set JPEG operation mode */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
	if (PTR_JPGC->info.operationType != JPEGDEC_PROGRESSIVE) {
		/* Set JPEG operation mode */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_E, 0);
	} else {
		/* Set JPEG operation mode */
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_E, 1);
	}

	/* Set spectral selection start coefficient */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_SS, PTR_JPGC->scan.Ss);

	/* Set spectral selection end coefficient */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_SE, PTR_JPGC->scan.Se);

	/* Set the point transform used in the preceding scan */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_AH, PTR_JPGC->scan.Ah);

	/* Set the point transform value */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_PJPEG_AL, PTR_JPGC->scan.Al);

	/* Set needed progressive parameters */
	if (PTR_JPGC->info.operationType == JPEGDEC_PROGRESSIVE) {
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set coefficient buffer base address\n"));
		coeffBuffer = PTR_JPGC->info.pCoeffBase.busAddress;
		/* non-interleaved */
		if (PTR_JPGC->info.nonInterleaved) {
			for (i = 0; i < PTR_JPGC->info.componentId; i++) {
				coeffBuffer += (JPEGDEC_COEFF_SIZE *
								PTR_JPGC->frame.numBlocks[i]);
			}
			SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_PJPEG_COEFF_BUF,
						 coeffBuffer);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_DIS, 0);
		}
		/* interleaved components */
		else {
			SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_PJPEG_COEFF_BUF,
						 coeffBuffer);
			coeffBuffer += (JPEGDEC_COEFF_SIZE) * PTR_JPGC->frame.numBlocks[0];
			SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_PJPEG_DCCB_BASE,
						 coeffBuffer);
			coeffBuffer += (JPEGDEC_COEFF_SIZE) * PTR_JPGC->frame.numBlocks[1];
			SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_PJPEG_DCCR_BASE,
						 coeffBuffer);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_OUT_DIS, 1);
		}
	}

	/* write "length amounts" */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write VLC length amounts to register\n"));
	JpegDecWriteLenBitsProgressive(PTR_JPGC);

	/* Create AC/DC/QP tables for HW */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write AC,DC,QP tables to base\n"));
	JpegDecWriteTablesProgressive(PTR_JPGC);

	/* Select which tables the chromas use */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Select chroma AC,DC tables\n"));
	JpegDecChromaTableSelectors(PTR_JPGC);

	/* write table base */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set AC,DC,QP table base address\n"));
	SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_QTABLE_BASE,
				 PTR_JPGC->frame.pTableBase.busAddress);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_JPEG_QTABLES,
				   PTR_JPGC->info.amountOfQTables);

	if (PTR_JPGC->info.sliceMbSetValue) {
		/*************** Set swreg15 data ************/
		JPEGDEC_TRACE_INTERNAL(("INTERNAL CONTINUE: Set slice/full mode: 0 full; other = slice\n"));
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_JPEG_SLICE_H,
					   PTR_JPGC->info.sliceHeight);
	}

	/* set restart interval */
	if (PTR_JPGC->frame.Ri) {
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_SYNC_MARKER_E, 1);
		SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_REFER13_BASE,
					 PTR_JPGC->frame.Ri);
	} else {
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_SYNC_MARKER_E, 0);
	}

	PTR_JPGC->asicRunning = 1;

	/* Flush regs to hw register */
	JpegFlushRegs(PTR_JPGC);
	DWLWriteReg(pJpegDecCont->dwl, pJpegDecCont->coreID, 4 * 101, PTR_JPGC->jpegRegs[101]);

	/* Enable jpeg mode and set slice mode */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Enable jpeg\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DEC_E, 1);
	DWLEnableHW(PTR_JPGC->dwl, PTR_JPGC->coreID, 4 * 1, PTR_JPGC->jpegRegs[1]);

#ifdef JPEGDEC_ASIC_TRACE
	{
		JPEGDEC_TRACE_INTERNAL(("PROGRESSIVE CONTINUE: REGS BEFORE IRQ CLEAN\n"));
		PrintJPEGReg(pJpegDecCont->jpegRegs);
	}
#endif /* #ifdef JPEGDEC_ASIC_TRACE */

}

/*------------------------------------------------------------------------------

        Function name: JpegDecSetHwStrmParams

        Functional description:
          set up hw stream start position

        Inputs:
          JpegDecContainer *pJpegDecCont

        Outputs:
          void

------------------------------------------------------------------------------*/
static void JpegDecSetHwStrmParams(JpegDecContainer *pJpegDecCont)
{

#define PTR_JPGC    pJpegDecCont
#define JPG_STR     pJpegDecCont->stream

	g1_addr_t addrTmp = 0;
	u32 amountOfStream = 0;

	/* calculate and set stream start address to hw */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: read bits %d\n", JPG_STR.readBits));
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: read bytes %d\n", JPG_STR.readBits / 8));
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Stream bus start 0x%08x\n",
							JPG_STR.streamBus));
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Stream virtual start 0x%08x\n",
							JPG_STR.pStartOfStream));

	/* calculate and set stream start address to hw */
	addrTmp = ((g1_addr_t) JPG_STR.streamBus + ((g1_addr_t) JPG_STR.pStartOfStream & 0x3) +
			   (g1_addr_t)(JPG_STR.pCurrPos - JPG_STR.pStartOfStream)) & (~7);

	SET_ADDR_REG(PTR_JPGC->jpegRegs, HWIF_RLC_VLC_BASE, addrTmp);

	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Stream bus start 0x%08x\n",
							JPG_STR.streamBus));
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Start Addr 0x%08x\n",
							GetJpegDecStreamStartAddress(PTR_JPGC->jpegRegs)));

	/* calculate and set stream start bit to hw */

	/* change current pos to bus address style */
	/* remove three lowest bits and add the difference to bitPosInWord */
	/* used as bit pos in word not as bit pos in byte actually... */
	switch ((g1_addr_t) JPG_STR.pCurrPos & (7)) {
	case 0:
		break;
	case 1:
		JPG_STR.bitPosInByte += 8;
		break;
	case 2:
		JPG_STR.bitPosInByte += 16;
		break;
	case 3:
		JPG_STR.bitPosInByte += 24;
		break;
	case 4:
		JPG_STR.bitPosInByte += 32;
		break;
	case 5:
		JPG_STR.bitPosInByte += 40;
		break;
	case 6:
		JPG_STR.bitPosInByte += 48;
		break;
	case 7:
		JPG_STR.bitPosInByte += 56;
		break;
	default:
		ASSERT(0);
		break;
	}

	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_STRM_START_BIT,
				   JPG_STR.bitPosInByte);

	/* set up stream length for HW.
	 * length = size of original buffer - stream we already decoded in SW */
	JPG_STR.pCurrPos = (u8 *)((g1_addr_t) JPG_STR.pCurrPos & (~7));

	if (PTR_JPGC->info.inputStreaming) {
		amountOfStream = (PTR_JPGC->info.inputBufferLen -
						  (u32)(JPG_STR.pCurrPos - JPG_STR.pStartOfStream));

		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_STREAM_LEN, amountOfStream);
	} else {
		amountOfStream = (JPG_STR.streamLength -
						  (u32)(JPG_STR.pCurrPos - JPG_STR.pStartOfStream));

		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_STREAM_LEN, amountOfStream);

		/* because no input streaming, frame should be ready during decoding this buffer */
		PTR_JPGC->info.streamEnd = 1;
	}

	/*************** Set swreg4 data ************/
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set stream last buffer bit\n"));
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_JPEG_STREAM_ALL,
				   PTR_JPGC->info.streamEnd);

	JPEGDEC_TRACE_INTERNAL(("INTERNAL: JPG_STR.streamLength %d\n",
							JPG_STR.streamLength));
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: JPG_STR.pCurrPos 0x%08x\n",
							(u32) JPG_STR.pCurrPos));
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: JPG_STR.pStartOfStream 0x%08x\n",
							(u32) JPG_STR.pStartOfStream));
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: JPG_STR.bitPosInByte 0x%08x\n",
							JPG_STR.bitPosInByte));

	return;

#undef JPG_STR
#undef PTR_JPGC
}

/*------------------------------------------------------------------------------

        Function name: JpegDecAllocateResidual

        Functional description:
          Allocates residual buffer

        Inputs:
          JpegDecContainer *pJpegDecCont  Pointer to DecData structure

        Outputs:
          OK
          JPEGDEC_MEMFAIL

------------------------------------------------------------------------------*/
JpegDecRet JpegDecAllocateResidual(JpegDecContainer *pJpegDecCont)
{
#define PTR_JPGC   pJpegDecCont

	i32 tmp = JPEGDEC_ERROR;
	u32 numBlocks = 0;
	u32 i;
	u32 tableSize = 0;

	ASSERT(PTR_JPGC);

	if (PTR_JPGC->info.operationType == JPEGDEC_PROGRESSIVE) {
		for (i = 0; i < PTR_JPGC->frame.Nf; i++) {
			numBlocks += PTR_JPGC->frame.numBlocks[i];
		}

		/* allocate coefficient buffer */
		tmp = DWLMallocLinear(PTR_JPGC->dwl, (sizeof(u8) * (JPEGDEC_COEFF_SIZE *
											  numBlocks)),
							  &(PTR_JPGC->info.pCoeffBase));
		if (tmp == -1) {
			return (JPEGDEC_MEMFAIL);
		}
#ifdef PJPEG_COMPONENT_TRACE
		pjpegCoeffBase = PTR_JPGC->info.pCoeffBase.virtualAddress;
		pjpegCoeffSize = numBlocks * JPEGDEC_COEFF_SIZE;
#endif

		JPEGDEC_TRACE_INTERNAL(("ALLOCATE: COEFF virtual %x bus %x\n",
								(u32) PTR_JPGC->info.pCoeffBase.virtualAddress,
								PTR_JPGC->info.pCoeffBase.busAddress));
		if (PTR_JPGC->frame.Nf > 1) {
			tmp = DWLMallocLinear(PTR_JPGC->dwl, sizeof(u8) * 100,
								  &PTR_JPGC->info.tmpStrm);
			if (tmp == -1) {
				return (JPEGDEC_MEMFAIL);
			}
		}
	}

	/* QP/VLC memory size */
	if (PTR_JPGC->info.operationType == JPEGDEC_PROGRESSIVE) {
		tableSize = JPEGDEC_PROGRESSIVE_TABLE_SIZE;
	} else {
		tableSize = JPEGDEC_BASELINE_TABLE_SIZE;
	}

	/* allocate VLC/QP table */
	if (PTR_JPGC->frame.pTableBase.virtualAddress == NULL) {
		tmp =
			DWLMallocLinear(PTR_JPGC->dwl, (sizeof(u8) * tableSize),
							&(PTR_JPGC->frame.pTableBase));
		if (tmp == -1) {
			return (JPEGDEC_MEMFAIL);
		}
	}

	JPEGDEC_TRACE_INTERNAL(("ALLOCATE: VLC/QP virtual %x bus %x\n",
							(u32) PTR_JPGC->frame.pTableBase.virtualAddress,
							PTR_JPGC->frame.pTableBase.busAddress));

	if (PTR_JPGC->ppInstance != NULL) {
		PTR_JPGC->ppConfigQuery.tiledMode = 0;
		PTR_JPGC->PPConfigQuery(PTR_JPGC->ppInstance, &PTR_JPGC->ppConfigQuery);

		PTR_JPGC->ppControl.usePipeline =
			PTR_JPGC->ppConfigQuery.pipelineAccepted;

		if (!PTR_JPGC->ppControl.usePipeline) {
			PTR_JPGC->image.sizeLuma = (PTR_JPGC->info.X * PTR_JPGC->info.Y);
			if (PTR_JPGC->image.sizeChroma) {
				if (PTR_JPGC->info.yCbCrMode == JPEGDEC_YUV420) {
					PTR_JPGC->image.sizeChroma = (PTR_JPGC->image.sizeLuma / 2);
				} else if (PTR_JPGC->info.yCbCrMode == JPEGDEC_YUV422 ||
						   PTR_JPGC->info.yCbCrMode == JPEGDEC_YUV440) {
					PTR_JPGC->image.sizeChroma = PTR_JPGC->image.sizeLuma;
				}
			}
		}
	}

	/* if pipelined PP -> decoder's output is not written external memory */
	if (PTR_JPGC->ppInstance == NULL ||
		(PTR_JPGC->ppInstance != NULL && !PTR_JPGC->ppControl.usePipeline)) {
		if (PTR_JPGC->info.givenOutLuma.virtualAddress == NULL) {
			/* allocate luminance output */
			if (PTR_JPGC->asicBuff.outLumaBuffer.virtualAddress == NULL) {
				tmp =
					DWLMallocRefFrm(PTR_JPGC->dwl, (PTR_JPGC->image.sizeLuma),
									&(PTR_JPGC->asicBuff.outLumaBuffer));
				if (tmp == -1) {
					return (JPEGDEC_MEMFAIL);
				}
			}

			/* luma bus address to output */
			PTR_JPGC->info.outLuma = PTR_JPGC->asicBuff.outLumaBuffer;
		} else {
			PTR_JPGC->asicBuff.outLumaBuffer.virtualAddress =
				PTR_JPGC->info.givenOutLuma.virtualAddress;
			PTR_JPGC->asicBuff.outLumaBuffer.busAddress =
				PTR_JPGC->info.givenOutLuma.busAddress;

			/* luma bus address to output */
			PTR_JPGC->info.outLuma = PTR_JPGC->asicBuff.outLumaBuffer;

			/* flag to release */
			PTR_JPGC->info.userAllocMem = 1;
		}

		JPEGDEC_TRACE_INTERNAL(("ALLOCATE: Luma virtual %lx bus %lx\n",
								PTR_JPGC->asicBuff.outLumaBuffer.
								virtualAddress,
								PTR_JPGC->asicBuff.outLumaBuffer.busAddress));

		/* allocate chrominance output */
		if (PTR_JPGC->image.sizeChroma) {
			if (PTR_JPGC->info.givenOutChroma.virtualAddress == NULL) {
				if (PTR_JPGC->info.operationType != JPEGDEC_BASELINE) {
					if (PTR_JPGC->asicBuff.outChromaBuffer.virtualAddress == NULL) {
						tmp =
							DWLMallocRefFrm(PTR_JPGC->dwl,
											(PTR_JPGC->image.sizeChroma / 2),
											&(PTR_JPGC->asicBuff.outChromaBuffer));
						if (tmp == -1) {
							return (JPEGDEC_MEMFAIL);
						}
					}

					if (PTR_JPGC->asicBuff.outChromaBuffer2.virtualAddress == NULL) {
						tmp =
							DWLMallocRefFrm(PTR_JPGC->dwl,
											(PTR_JPGC->image.sizeChroma / 2),
											&(PTR_JPGC->asicBuff.outChromaBuffer2));
						if (tmp == -1) {
							return (JPEGDEC_MEMFAIL);
						}
					}
				} else {
					if (PTR_JPGC->asicBuff.outChromaBuffer.virtualAddress == NULL) {
						tmp =
							DWLMallocRefFrm(PTR_JPGC->dwl,
											(PTR_JPGC->image.sizeChroma),
											&(PTR_JPGC->asicBuff.outChromaBuffer));
						if (tmp == -1) {
							return (JPEGDEC_MEMFAIL);
						}
					}

					PTR_JPGC->asicBuff.outChromaBuffer2.virtualAddress = NULL;
					PTR_JPGC->asicBuff.outChromaBuffer2.busAddress = 0;
				}
			} else {
				PTR_JPGC->asicBuff.outChromaBuffer.virtualAddress =
					PTR_JPGC->info.givenOutChroma.virtualAddress;
				PTR_JPGC->asicBuff.outChromaBuffer.busAddress =
					PTR_JPGC->info.givenOutChroma.busAddress;
				PTR_JPGC->asicBuff.outChromaBuffer2.virtualAddress =
					PTR_JPGC->info.givenOutChroma2.virtualAddress;
				PTR_JPGC->asicBuff.outChromaBuffer2.busAddress =
					PTR_JPGC->info.givenOutChroma2.busAddress;

			}

			/* chroma bus address to output */
			PTR_JPGC->info.outChroma = PTR_JPGC->asicBuff.outChromaBuffer;
			PTR_JPGC->info.outChroma2 = PTR_JPGC->asicBuff.outChromaBuffer2;

			JPEGDEC_TRACE_INTERNAL(("ALLOCATE: Chroma virtual %lx bus %lx\n",
									PTR_JPGC->asicBuff.outChromaBuffer.
									virtualAddress,
									PTR_JPGC->asicBuff.outChromaBuffer.
									busAddress));
		}
	}

#ifdef JPEGDEC_RESET_OUTPUT
	{
		(void) DWLmemset(PTR_JPGC->asicBuff.outLumaBuffer.virtualAddress,
						 128, PTR_JPGC->image.sizeLuma);
		if (PTR_JPGC->image.sizeChroma) {
			if (PTR_JPGC->info.operationType != JPEGDEC_BASELINE) {
				(void) DWLmemset(PTR_JPGC->asicBuff.outChromaBuffer.
								 virtualAddress, 128,
								 PTR_JPGC->image.sizeChroma / 2);
				(void) DWLmemset(PTR_JPGC->asicBuff.outChromaBuffer2.
								 virtualAddress, 128,
								 PTR_JPGC->image.sizeChroma / 2);
			} else
				(void) DWLmemset(PTR_JPGC->asicBuff.outChromaBuffer.
								 virtualAddress, 128,
								 PTR_JPGC->image.sizeChroma);
		}
		(void) DWLmemset(PTR_JPGC->frame.pTableBase.virtualAddress, 0,
						 (sizeof(u8) * tableSize));
		if (PTR_JPGC->info.operationType == JPEGDEC_PROGRESSIVE) {
			(void) DWLmemset(PTR_JPGC->info.pCoeffBase.virtualAddress, 0,
							 (sizeof(u8) * JPEGDEC_COEFF_SIZE * numBlocks));
		}
	}
#endif /* #ifdef JPEGDEC_RESET_OUTPUT */

	return JPEGDEC_OK;

#undef PTR_JPGC
}

/*------------------------------------------------------------------------------

        Function name: JpegDecSliceSizeCalculation

        Functional description:
          Calculates slice size

        Inputs:
          JpegDecContainer *pJpegDecCont

        Outputs:
          void

------------------------------------------------------------------------------*/
void JpegDecSliceSizeCalculation(JpegDecContainer *pJpegDecCont)
{
#define PTR_JPGC   pJpegDecCont

	if (((PTR_JPGC->info.SliceCount +
		  1) * (PTR_JPGC->info.sliceMbSetValue * 16)) > PTR_JPGC->info.Y) {
		PTR_JPGC->info.sliceHeight = ((PTR_JPGC->info.Y / 16) -
									  (PTR_JPGC->info.SliceCount *
									   PTR_JPGC->info.sliceHeight));
	} else {
		/* TODO! other sampling formats also than YUV420 */
		if (PTR_JPGC->info.operationType == JPEGDEC_PROGRESSIVE &&
			PTR_JPGC->info.componentId != 0) {
			PTR_JPGC->info.sliceHeight = PTR_JPGC->info.sliceMbSetValue / 2;
		} else {
			PTR_JPGC->info.sliceHeight = PTR_JPGC->info.sliceMbSetValue;
		}
	}
}

/*------------------------------------------------------------------------------

        Function name: JpegDecWriteTables

        Functional description:
          Writes q/ac/dc tables to the HW format as specified in HW regs

        Inputs:
          JpegDecContainer *pJpegDecCont

        Outputs:
          void

------------------------------------------------------------------------------*/
static void JpegDecWriteTables(JpegDecContainer *pJpegDecCont)
{

#define PTR_JPGC    pJpegDecCont
#define JPG_SCN     pJpegDecCont->scan
#define JPG_VLC     pJpegDecCont->vlc
#define JPG_QTB     pJpegDecCont->quant
#define JPG_FRM     pJpegDecCont->frame

	u32 i, j = 0;
	u32 shifter = 32;
	u32 tableWord = 0;
	u32 tableValue = 0;
	u8 tableTmp[64] = { 0 };
	u32 *pTableBase = NULL;

	ASSERT(PTR_JPGC);
	ASSERT(PTR_JPGC->frame.pTableBase.virtualAddress);
	ASSERT(PTR_JPGC->frame.pTableBase.busAddress);
	ASSERT(PTR_JPGC->frame.pTableBase.size);

	pTableBase = PTR_JPGC->frame.pTableBase.virtualAddress;

	/* QP tables for all components */
	for (j = 0; j < PTR_JPGC->info.amountOfQTables; j++) {
		if ((JPG_FRM.component[j].Tq) == 0) {
			for (i = 0; i < 64; i++) {
				tableTmp[zzOrder[i]] = (u8) JPG_QTB.table0[i];
			}

			/* update shifter */
			shifter = 32;

			for (i = 0; i < 64; i++) {
				shifter -= 8;

				if (shifter == 24) {
					tableWord = (tableTmp[i] << shifter);
				} else {
					tableWord |= (tableTmp[i] << shifter);
				}

				if (shifter == 0) {
					*(pTableBase) = tableWord;
					pTableBase++;
					shifter = 32;
				}
			}
		} else {
			for (i = 0; i < 64; i++) {
				tableTmp[zzOrder[i]] = (u8) JPG_QTB.table1[i];
			}

			/* update shifter */
			shifter = 32;

			for (i = 0; i < 64; i++) {
				shifter -= 8;

				if (shifter == 24) {
					tableWord = (tableTmp[i] << shifter);
				} else {
					tableWord |= (tableTmp[i] << shifter);
				}

				if (shifter == 0) {
					*(pTableBase) = tableWord;
					pTableBase++;
					shifter = 32;
				}
			}
		}
	}

	/* update shifter */
	shifter = 32;

	if (PTR_JPGC->info.yCbCrMode != JPEGDEC_YUV400) {
		/* this trick is done because hw always wants luma table as ac hw table 1 */
		if (JPG_SCN.Ta[0] == 0) {
			/* Write AC Table 1 (as specified in HW regs)
			 * NOTE: Not the same as actable[1] (as specified in JPEG Spec) */
			JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write tables: AC1 (luma)\n"));
			if (JPG_VLC.acTable0.vals) {
				for (i = 0; i < 162; i++) {
					if (i < JPG_VLC.acTable0.tableLength) {
						tableValue = (u8) JPG_VLC.acTable0.vals[i];
					} else {
						tableValue = 0;
					}

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			} else {
				for (i = 0; i < 162; i++) {
					tableWord = 0;

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			}
			/* Write AC Table 2 */
			JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write tables: AC2 (not-luma)\n"));
			if (JPG_VLC.acTable1.vals) {
				for (i = 0; i < 162; i++) {
					if (i < JPG_VLC.acTable1.tableLength) {
						tableValue = (u8) JPG_VLC.acTable1.vals[i];
					} else {
						tableValue = 0;
					}

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			} else {
				for (i = 0; i < 162; i++) {
					tableWord = 0;

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			}
		} else {
			/* Write AC Table 1 (as specified in HW regs)
			 * NOTE: Not the same as actable[1] (as specified in JPEG Spec) */

			if (JPG_VLC.acTable1.vals) {
				JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write tables: AC1 (luma)\n"));
				for (i = 0; i < 162; i++) {
					if (i < JPG_VLC.acTable1.tableLength) {
						tableValue = (u8) JPG_VLC.acTable1.vals[i];
					} else {
						tableValue = 0;
					}

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			} else {
				for (i = 0; i < 162; i++) {
					tableWord = 0;

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			}

			/* Write AC Table 2 */

			if (JPG_VLC.acTable0.vals) {
				JPEGDEC_TRACE_INTERNAL(("INTERNAL: writeTables: AC2 (not-luma)\n"));
				for (i = 0; i < 162; i++) {
					if (i < JPG_VLC.acTable0.tableLength) {
						tableValue = (u8) JPG_VLC.acTable0.vals[i];
					} else {
						tableValue = 0;
					}

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			} else {
				for (i = 0; i < 162; i++) {
					tableWord = 0;

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			}
		}

		/* this trick is done because hw always wants luma table as dc hw table 1 */
		if (JPG_SCN.Td[0] == 0) {
			if (JPG_VLC.dcTable0.vals) {
				for (i = 0; i < 12; i++) {
					if (i < JPG_VLC.dcTable0.tableLength) {
						tableValue = (u8) JPG_VLC.dcTable0.vals[i];
					} else {
						tableValue = 0;
					}

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			} else {
				for (i = 0; i < 12; i++) {
					tableWord = 0;

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			}

			if (JPG_VLC.dcTable1.vals) {
				for (i = 0; i < 12; i++) {
					if (i < JPG_VLC.dcTable1.tableLength) {
						tableValue = (u8) JPG_VLC.dcTable1.vals[i];
					} else {
						tableValue = 0;
					}

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			} else {
				for (i = 0; i < 12; i++) {
					tableWord = 0;

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			}

		} else {
			if (JPG_VLC.dcTable1.vals) {
				for (i = 0; i < 12; i++) {
					if (i < JPG_VLC.dcTable1.tableLength) {
						tableValue = (u8) JPG_VLC.dcTable1.vals[i];
					} else {
						tableValue = 0;
					}

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			} else {
				for (i = 0; i < 12; i++) {
					tableWord = 0;

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			}

			if (JPG_VLC.dcTable0.vals) {
				for (i = 0; i < 12; i++) {
					if (i < JPG_VLC.dcTable0.tableLength) {
						tableValue = (u8) JPG_VLC.dcTable0.vals[i];
					} else {
						tableValue = 0;
					}

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			} else {
				for (i = 0; i < 12; i++) {
					tableWord = 0;

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			}
		}
	} else { /* YUV400 */
		if (!PTR_JPGC->info.nonInterleavedScanReady) {
			/* this trick is done because hw always wants luma table as ac hw table 1 */
			if (JPG_SCN.Ta[0] == 0) {
				/* Write AC Table 1 (as specified in HW regs)
				 * NOTE: Not the same as actable[1] (as specified in JPEG Spec) */
				JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write tables: AC1 (luma)\n"));
				if (JPG_VLC.acTable0.vals) {
					for (i = 0; i < 162; i++) {
						if (i < JPG_VLC.acTable0.tableLength) {
							tableValue = (u8) JPG_VLC.acTable0.vals[i];
						} else {
							tableValue = 0;
						}

						if (shifter == 32) {
							tableWord = (tableValue << (shifter - 8));
						} else {
							tableWord |= (tableValue << (shifter - 8));
						}

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				} else {
					for (i = 0; i < 162; i++) {
						tableWord = 0;

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				}

				/* Write AC Table 2 */
				JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write zero table (YUV400): \n"));
				for (i = 0; i < 162; i++) {
					tableValue = 0;

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			} else {
				/* Write AC Table 1 (as specified in HW regs)
				 * NOTE: Not the same as actable[1] (as specified in JPEG Spec) */

				if (JPG_VLC.acTable1.vals) {
					JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write tables: AC1 (luma)\n"));
					for (i = 0; i < 162; i++) {
						if (i < JPG_VLC.acTable1.tableLength) {
							tableValue = (u8) JPG_VLC.acTable1.vals[i];
						} else {
							tableValue = 0;
						}

						if (shifter == 32) {
							tableWord = (tableValue << (shifter - 8));
						} else {
							tableWord |= (tableValue << (shifter - 8));
						}

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				} else {
					for (i = 0; i < 162; i++) {
						tableWord = 0;

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				}

				/* Write AC Table 2 */
				JPEGDEC_TRACE_INTERNAL(("INTERNAL: writeTables: padding zero (YUV400)\n"));
				for (i = 0; i < 162; i++) {
					tableValue = 0;

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			}

			/* this trick is done because hw always wants luma table as dc hw table 1 */
			if (JPG_SCN.Td[0] == 0) {
				if (JPG_VLC.dcTable0.vals) {
					for (i = 0; i < 12; i++) {
						if (i < JPG_VLC.dcTable0.tableLength) {
							tableValue = (u8) JPG_VLC.dcTable0.vals[i];
						} else {
							tableValue = 0;
						}

						if (shifter == 32) {
							tableWord = (tableValue << (shifter - 8));
						} else {
							tableWord |= (tableValue << (shifter - 8));
						}

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				} else {
					for (i = 0; i < 12; i++) {
						tableWord = 0;

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				}

				for (i = 0; i < 12; i++) {
					tableValue = 0;

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			} else {
				if (JPG_VLC.dcTable1.vals) {
					for (i = 0; i < 12; i++) {
						if (i < JPG_VLC.dcTable1.tableLength) {
							tableValue = (u8) JPG_VLC.dcTable1.vals[i];
						} else {
							tableValue = 0;
						}

						if (shifter == 32) {
							tableWord = (tableValue << (shifter - 8));
						} else {
							tableWord |= (tableValue << (shifter - 8));
						}

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				} else {
					for (i = 0; i < 12; i++) {
						tableWord = 0;

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				}

				for (i = 0; i < 12; i++) {
					tableValue = 0;

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			}
		} else {
			/* this trick is done because hw always wants luma table as ac hw table 1 */
			if (JPG_SCN.Ta[PTR_JPGC->info.componentId] == 0) {
				/* Write AC Table 1 (as specified in HW regs)
				 * NOTE: Not the same as actable[1] (as specified in JPEG Spec) */
				JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write tables: AC1 (luma)\n"));
				if (JPG_VLC.acTable0.vals) {
					for (i = 0; i < 162; i++) {
						if (i < JPG_VLC.acTable0.tableLength) {
							tableValue = (u8) JPG_VLC.acTable0.vals[i];
						} else {
							tableValue = 0;
						}

						if (shifter == 32) {
							tableWord = (tableValue << (shifter - 8));
						} else {
							tableWord |= (tableValue << (shifter - 8));
						}

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				} else {
					for (i = 0; i < 162; i++) {
						tableWord = 0;

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				}

				/* Write AC Table 2 */
				JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write zero table (YUV400): \n"));
				for (i = 0; i < 162; i++) {
					tableValue = 0;

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			} else {
				/* Write AC Table 1 (as specified in HW regs)
				 * NOTE: Not the same as actable[1] (as specified in JPEG Spec) */

				if (JPG_VLC.acTable1.vals) {
					JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write tables: AC1 (luma)\n"));
					for (i = 0; i < 162; i++) {
						if (i < JPG_VLC.acTable1.tableLength) {
							tableValue = (u8) JPG_VLC.acTable1.vals[i];
						} else {
							tableValue = 0;
						}

						if (shifter == 32) {
							tableWord = (tableValue << (shifter - 8));
						} else {
							tableWord |= (tableValue << (shifter - 8));
						}

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				} else {
					for (i = 0; i < 162; i++) {
						tableWord = 0;

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				}

				/* Write AC Table 2 */
				JPEGDEC_TRACE_INTERNAL(("INTERNAL: writeTables: padding zero (YUV400)\n"));
				for (i = 0; i < 162; i++) {
					tableValue = 0;

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			}

			/* this trick is done because hw always wants luma table as dc hw table 1 */
			if (JPG_SCN.Td[PTR_JPGC->info.componentId] == 0) {
				if (JPG_VLC.dcTable0.vals) {
					for (i = 0; i < 12; i++) {
						if (i < JPG_VLC.dcTable0.tableLength) {
							tableValue = (u8) JPG_VLC.dcTable0.vals[i];
						} else {
							tableValue = 0;
						}

						if (shifter == 32) {
							tableWord = (tableValue << (shifter - 8));
						} else {
							tableWord |= (tableValue << (shifter - 8));
						}

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				} else {
					for (i = 0; i < 12; i++) {
						tableWord = 0;

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				}

				for (i = 0; i < 12; i++) {
					tableValue = 0;

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			} else {
				if (JPG_VLC.dcTable1.vals) {
					for (i = 0; i < 12; i++) {
						if (i < JPG_VLC.dcTable1.tableLength) {
							tableValue = (u8) JPG_VLC.dcTable1.vals[i];
						} else {
							tableValue = 0;
						}

						if (shifter == 32) {
							tableWord = (tableValue << (shifter - 8));
						} else {
							tableWord |= (tableValue << (shifter - 8));
						}

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				} else {
					for (i = 0; i < 12; i++) {
						tableWord = 0;

						shifter -= 8;

						if (shifter == 0) {
							*(pTableBase) = tableWord;
							pTableBase++;
							shifter = 32;
						}
					}
				}

				for (i = 0; i < 12; i++) {
					tableValue = 0;

					if (shifter == 32) {
						tableWord = (tableValue << (shifter - 8));
					} else {
						tableWord |= (tableValue << (shifter - 8));
					}

					shifter -= 8;

					if (shifter == 0) {
						*(pTableBase) = tableWord;
						pTableBase++;
						shifter = 32;
					}
				}
			}
		}

	}

	for (i = 0; i < 4; i++) {
		tableValue = 0;

		if (shifter == 32) {
			tableWord = (tableValue << (shifter - 8));
		} else {
			tableWord |= (tableValue << (shifter - 8));
		}

		shifter -= 8;

		if (shifter == 0) {
			*(pTableBase) = tableWord;
			pTableBase++;
			shifter = 32;
		}
	}

#undef JPG_SCN
#undef JPG_VLC
#undef JPG_QTB
#undef JPG_FRM
#undef PTR_JPGC

}

/*------------------------------------------------------------------------------
        Function name: JpegDecWriteTablesNonInterleaved

        Functional description:
          Writes q/ac/dc tables to the HW format as specified in HW regs

        Inputs:
          JpegDecContainer *pJpegDecCont

        Outputs:
          void

------------------------------------------------------------------------------*/
static void JpegDecWriteTablesNonInterleaved(JpegDecContainer *pJpegDecCont)
{

#define PTR_JPGC    pJpegDecCont
#define JPG_SCN     pJpegDecCont->scan
#define JPG_VLC     pJpegDecCont->vlc
#define JPG_QTB     pJpegDecCont->quant
#define JPG_FRM     pJpegDecCont->frame

	u32 i, j = 0;
	u32 tableWord = 0;
	u8 tableTmp[64] = { 0 };
	u8 *pTmp;
	u32 *pTableBase = NULL;
	u32 first, count;
	u32 len, numWords;
	u32 *vals;
	u32 *pTable;
	u32 qpTableBase = 0;

	ASSERT(PTR_JPGC);
	ASSERT(PTR_JPGC->frame.pTableBase.virtualAddress);
	ASSERT(PTR_JPGC->frame.pTableBase.busAddress);
	ASSERT(PTR_JPGC->frame.pTableBase.size);
	ASSERT(PTR_JPGC->info.nonInterleaved);

	/* Reset the table memory */
	(void) DWLmemset(PTR_JPGC->frame.pTableBase.virtualAddress, 0,
					 (sizeof(u8) * JPEGDEC_BASELINE_TABLE_SIZE));

	pTableBase = PTR_JPGC->frame.pTableBase.virtualAddress;

	first = PTR_JPGC->info.componentId;
	count = 1;

	/* QP tables for all components */
	for (j = first; j < first + count; j++) {
		if ((JPG_FRM.component[j].Tq) == 0) {
			pTable = JPG_QTB.table0;
		} else {
			pTable = JPG_QTB.table1;
		}

		for (i = 0; i < 64; i++) {
			tableTmp[zzOrder[i]] = (u8) pTable[i];
		}

		pTmp = tableTmp;
		for (i = 0; i < 16; i++) {
			tableWord = (pTmp[0] << 24) | (pTmp[1] << 16) |
						(pTmp[2] << 8) | (pTmp[3] << 0);;

			*pTableBase++ = tableWord;
			pTmp += 4;
		}
	}

	/* AC table */
	for (i = first; i < first + count; i++) {
		numWords = 162;
		switch (JPG_SCN.Ta[i]) {
		case 0:
			vals = JPG_VLC.acTable0.vals;
			len = JPG_VLC.acTable0.tableLength;
			break;
		case 1:
			vals = JPG_VLC.acTable1.vals;
			len = JPG_VLC.acTable1.tableLength;
			break;
		case 2:
			vals = JPG_VLC.acTable2.vals;
			len = JPG_VLC.acTable2.tableLength;
			break;
		default:
			vals = JPG_VLC.acTable3.vals;
			len = JPG_VLC.acTable3.tableLength;
			break;
		}

		/* set pointer */
		if (count == 3) {
			qpTableBase = 0;
		} else {
			qpTableBase = JPEGDEC_QP_BASE;
		}

		pTableBase =
			&PTR_JPGC->frame.pTableBase.virtualAddress[JPEGDEC_AC1_BASE -
									 qpTableBase];

		for (j = 0; j < numWords; j++) {
			tableWord <<= 8;
			if (j < len) {
				tableWord |= vals[j];
			}

			if ((j & 0x3) == 0x3) {
				*pTableBase++ = tableWord;
			}
		}

		/* fill to border */
		numWords = 164;
		len = 164;
		for (j = 162; j < numWords; j++) {
			tableWord <<= 8;
			if (j < len) {
				tableWord |= 0;
			}

			if ((j & 0x3) == 0x3) {
				*pTableBase++ = tableWord;
			}
		}
	}

	/* DC table */
	for (i = first; i < first + count; i++) {
		numWords = 12;
		switch (JPG_SCN.Td[i]) {
		case 0:
			vals = JPG_VLC.dcTable0.vals;
			len = JPG_VLC.dcTable0.tableLength;
			break;
		case 1:
			vals = JPG_VLC.dcTable1.vals;
			len = JPG_VLC.dcTable1.tableLength;
			break;
		case 2:
			vals = JPG_VLC.dcTable2.vals;
			len = JPG_VLC.dcTable2.tableLength;
			break;
		default:
			vals = JPG_VLC.dcTable3.vals;
			len = JPG_VLC.dcTable3.tableLength;
			break;
		}

		/* set pointer */
		if (count == 3) {
			qpTableBase = 0;
		} else {
			qpTableBase = JPEGDEC_QP_BASE;
		}

		pTableBase =
			&PTR_JPGC->frame.pTableBase.virtualAddress[JPEGDEC_DC1_BASE -
									 qpTableBase];

		for (j = 0; j < numWords; j++) {
			tableWord <<= 8;
			if (j < len) {
				tableWord |= vals[j];
			}

			if ((j & 0x3) == 0x3) {
				*pTableBase++ = tableWord;
			}
		}
	}

	*pTableBase = 0;

#undef JPG_SCN
#undef JPG_VLC
#undef JPG_QTB
#undef JPG_FRM
#undef PTR_JPGC

}

/*------------------------------------------------------------------------------

        Function name: JpegDecWriteTablesProgressive

        Functional description:
          Writes q/ac/dc tables to the HW format as specified in HW regs

        Inputs:
          JpegDecContainer *pJpegDecCont

        Outputs:
          void

------------------------------------------------------------------------------*/
static void JpegDecWriteTablesProgressive(JpegDecContainer *pJpegDecCont)
{

#define PTR_JPGC    pJpegDecCont
#define JPG_SCN     pJpegDecCont->scan
#define JPG_VLC     pJpegDecCont->vlc
#define JPG_QTB     pJpegDecCont->quant
#define JPG_FRM     pJpegDecCont->frame

	u32 i, j = 0;
	u32 tableWord = 0;
	u8 tableTmp[64] = { 0 };
	u8 *pTmp;
	u32 *pTableBase = NULL;
	u32 first, count;
	u32 len, numWords;
	u32 *vals;
	u32 *pTable;
	u32 dcTable = 0;
	u32 qpTableBase = 0;

	ASSERT(PTR_JPGC);
	ASSERT(PTR_JPGC->frame.pTableBase.virtualAddress);
	ASSERT(PTR_JPGC->frame.pTableBase.busAddress);
	ASSERT(PTR_JPGC->frame.pTableBase.size);

	/* Reset the table memory */
	(void) DWLmemset(PTR_JPGC->frame.pTableBase.virtualAddress, 0,
					 (sizeof(u8) * JPEGDEC_PROGRESSIVE_TABLE_SIZE));

	pTableBase = PTR_JPGC->frame.pTableBase.virtualAddress;

	if (PTR_JPGC->info.nonInterleaved) {
		first = PTR_JPGC->info.componentId;
		count = 1;
	} else {
		first = 0;
		count = 3;
	}

	/* QP tables for all components */
	for (j = first; j < first + count; j++) {
		if ((JPG_FRM.component[j].Tq) == 0) {
			pTable = JPG_QTB.table0;
		} else {
			pTable = JPG_QTB.table1;
		}

		for (i = 0; i < 64; i++) {
			tableTmp[zzOrder[i]] = (u8) pTable[i];
		}

		pTmp = tableTmp;
		for (i = 0; i < 16; i++) {
			tableWord = (pTmp[0] << 24) | (pTmp[1] << 16) |
						(pTmp[2] << 8) | (pTmp[3] << 0);;

			*pTableBase++ = tableWord;
			pTmp += 4;
		}
	}

	/* if later stage DC ==> no need for table */
	if (PTR_JPGC->scan.Ah != 0 && PTR_JPGC->scan.Ss == 0) {
		return;
	}

	for (i = first; i < first + count; i++) {
		if (PTR_JPGC->scan.Ss == 0) { /* DC */
			dcTable = 1;
			numWords = 12;
			switch (JPG_SCN.Td[i]) {
			case 0:
				vals = JPG_VLC.dcTable0.vals;
				len = JPG_VLC.dcTable0.tableLength;
				break;
			case 1:
				vals = JPG_VLC.dcTable1.vals;
				len = JPG_VLC.dcTable1.tableLength;
				break;
			case 2:
				vals = JPG_VLC.dcTable2.vals;
				len = JPG_VLC.dcTable2.tableLength;
				break;
			default:
				vals = JPG_VLC.dcTable3.vals;
				len = JPG_VLC.dcTable3.tableLength;
				break;
			}
		} else {
			numWords = 162;
			switch (JPG_SCN.Ta[i]) {
			case 0:
				vals = JPG_VLC.acTable0.vals;
				len = JPG_VLC.acTable0.tableLength;
				break;
			case 1:
				vals = JPG_VLC.acTable1.vals;
				len = JPG_VLC.acTable1.tableLength;
				break;
			case 2:
				vals = JPG_VLC.acTable2.vals;
				len = JPG_VLC.acTable2.tableLength;
				break;
			default:
				vals = JPG_VLC.acTable3.vals;
				len = JPG_VLC.acTable3.tableLength;
				break;
			}
		}

		/* set pointer */
		if (count == 3) {
			qpTableBase = 0;
		} else {
			qpTableBase = JPEGDEC_QP_BASE;
		}

		if (dcTable) {
			/* interleaved || non-interleaved */
			if (count == 3) {
				if (i == 0)
					pTableBase =
						&PTR_JPGC->frame.pTableBase.
						virtualAddress[JPEGDEC_DC1_BASE - qpTableBase];
				else if (i == 1)
					pTableBase =
						&PTR_JPGC->frame.pTableBase.
						virtualAddress[JPEGDEC_DC2_BASE - qpTableBase];
				else
					pTableBase =
						&PTR_JPGC->frame.pTableBase.
						virtualAddress[JPEGDEC_DC3_BASE - qpTableBase];
			} else {
				pTableBase =
					&PTR_JPGC->frame.pTableBase.
					virtualAddress[JPEGDEC_DC1_BASE - qpTableBase];
			}
		} else {
			pTableBase =
				&PTR_JPGC->frame.pTableBase.virtualAddress[JPEGDEC_AC1_BASE -
										 qpTableBase];
		}

		for (j = 0; j < numWords; j++) {
			tableWord <<= 8;
			if (j < len) {
				tableWord |= vals[j];
			}

			if ((j & 0x3) == 0x3) {
				*pTableBase++ = tableWord;
			}
		}

		/* fill to border */
		if (i == 0 && dcTable == 0) {
			numWords = 164;
			len = 164;
			for (j = 162; j < numWords; j++) {
				tableWord <<= 8;
				if (j < len) {
					tableWord |= 0;
				}

				if ((j & 0x3) == 0x3) {
					*pTableBase++ = tableWord;
				}
			}
		}

		/* reset */
		dcTable = 0;
	}

	*pTableBase = 0;

#undef JPG_SCN
#undef JPG_VLC
#undef JPG_QTB
#undef JPG_FRM
#undef PTR_JPGC

}

/*------------------------------------------------------------------------------

        Function name: JpegDecChromaTableSelectors

        Functional description:
          select what tables chromas use

        Inputs:
          JpegDecContainer *pJpegDecCont

        Outputs:
          void

------------------------------------------------------------------------------*/
static void JpegDecChromaTableSelectors(JpegDecContainer *pJpegDecCont)
{

#define PTR_JPGC    pJpegDecCont
#define JPG_SCN     pJpegDecCont->scan
#define JPG_FRM     pJpegDecCont->frame

	/* this trick is done because hw always wants luma table as ac hw table 1 */
	if (JPG_SCN.Ta[0] == 0) {
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CR_AC_VLCTABLE, JPG_SCN.Ta[2]);
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CB_AC_VLCTABLE, JPG_SCN.Ta[1]);
	} else {
		if (JPG_SCN.Ta[0] == JPG_SCN.Ta[1]) {
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CB_AC_VLCTABLE, 0);
		} else {
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CB_AC_VLCTABLE, 1);
		}

		if (JPG_SCN.Ta[0] == JPG_SCN.Ta[2]) {
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CR_AC_VLCTABLE, 0);
		} else {
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CR_AC_VLCTABLE, 1);
		}
	}

	/* Third DC table selectors */
	if (PTR_JPGC->info.operationType != JPEGDEC_PROGRESSIVE) {
		if (JPG_SCN.Td[0] == 0) {
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CR_DC_VLCTABLE,
						   JPG_SCN.Td[2]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CB_DC_VLCTABLE,
						   JPG_SCN.Td[1]);
		} else {
			if (JPG_SCN.Td[0] == JPG_SCN.Td[1]) {
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CB_DC_VLCTABLE, 0);
			} else {
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CB_DC_VLCTABLE, 1);
			}

			if (JPG_SCN.Td[0] == JPG_SCN.Td[2]) {
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CR_DC_VLCTABLE, 0);
			} else {
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CR_DC_VLCTABLE, 1);
			}
		}

		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CR_DC_VLCTABLE3, 0);
		SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CB_DC_VLCTABLE3, 0);
	} else {
		/* if non-interleaved ==> decoding mode YUV400, uses table zero (0) */
		if (PTR_JPGC->info.nonInterleaved) {
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CR_DC_VLCTABLE, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CB_DC_VLCTABLE, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CR_DC_VLCTABLE3, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CB_DC_VLCTABLE3, 0);
		} else {
			/* if later stage DC ==> no need for table */
			if (PTR_JPGC->scan.Ah != 0 && PTR_JPGC->scan.Ss == 0) {
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CR_DC_VLCTABLE, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CR_DC_VLCTABLE3, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CB_DC_VLCTABLE, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CB_DC_VLCTABLE3, 0);
			} else {
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CR_DC_VLCTABLE, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CR_DC_VLCTABLE3, 1);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CB_DC_VLCTABLE, 1);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_CB_DC_VLCTABLE3, 0);
			}
		}
	}

	return;

#undef JPG_SCN
#undef JPG_FRM
#undef PTR_JPGC
}

/*------------------------------------------------------------------------------

        Function name: JpegDecWriteLenBits

        Functional description:
          tell hw how many vlc words of different lengths we have

        Inputs:
          JpegDecContainer *pJpegDecCont

        Outputs:
          void

------------------------------------------------------------------------------*/
static void JpegDecWriteLenBits(JpegDecContainer *pJpegDecCont)
{

#define PTR_JPGC    pJpegDecCont
#define JPG_SCN     pJpegDecCont->scan
#define JPG_VLC     pJpegDecCont->vlc
#define JPG_QTB     pJpegDecCont->quant
#define JPG_FRM     pJpegDecCont->frame

	VlcTable *pTable1 = NULL;
	VlcTable *pTable2 = NULL;

	/* first select the table we'll use */

	/* this trick is done because hw always wants luma table as ac hw table 1 */
	if (JPG_SCN.Ta[0] == 0) {

		pTable1 = &(JPG_VLC.acTable0);
		pTable2 = &(JPG_VLC.acTable1);

	} else {

		pTable1 = &(JPG_VLC.acTable1);
		pTable2 = &(JPG_VLC.acTable0);
	}

	ASSERT(pTable1);
	ASSERT(pTable2);

	/* write AC table 1 (luma) */

	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE1_CNT, pTable1->bits[0]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE2_CNT, pTable1->bits[1]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE3_CNT, pTable1->bits[2]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE4_CNT, pTable1->bits[3]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE5_CNT, pTable1->bits[4]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE6_CNT, pTable1->bits[5]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE7_CNT, pTable1->bits[6]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE8_CNT, pTable1->bits[7]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE9_CNT, pTable1->bits[8]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE10_CNT, pTable1->bits[9]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE11_CNT, pTable1->bits[10]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE12_CNT, pTable1->bits[11]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE13_CNT, pTable1->bits[12]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE14_CNT, pTable1->bits[13]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE15_CNT, pTable1->bits[14]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE16_CNT, pTable1->bits[15]);

	/* table AC2 (the not-luma table) */
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE1_CNT, pTable2->bits[0]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE2_CNT, pTable2->bits[1]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE3_CNT, pTable2->bits[2]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE4_CNT, pTable2->bits[3]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE5_CNT, pTable2->bits[4]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE6_CNT, pTable2->bits[5]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE7_CNT, pTable2->bits[6]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE8_CNT, pTable2->bits[7]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE9_CNT, pTable2->bits[8]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE10_CNT, pTable2->bits[9]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE11_CNT, pTable2->bits[10]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE12_CNT, pTable2->bits[11]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE13_CNT, pTable2->bits[12]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE14_CNT, pTable2->bits[13]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE15_CNT, pTable2->bits[14]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC2_CODE16_CNT, pTable2->bits[15]);

	if (JPG_SCN.Td[0] == 0) {

		pTable1 = &(JPG_VLC.dcTable0);
		pTable2 = &(JPG_VLC.dcTable1);

	} else {

		pTable1 = &(JPG_VLC.dcTable1);
		pTable2 = &(JPG_VLC.dcTable0);
	}

	ASSERT(pTable1);
	ASSERT(pTable2);

	/* write DC table 1 (luma) */
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE1_CNT, pTable1->bits[0]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE2_CNT, pTable1->bits[1]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE3_CNT, pTable1->bits[2]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE4_CNT, pTable1->bits[3]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE5_CNT, pTable1->bits[4]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE6_CNT, pTable1->bits[5]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE7_CNT, pTable1->bits[6]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE8_CNT, pTable1->bits[7]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE9_CNT, pTable1->bits[8]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE10_CNT, pTable1->bits[9]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE11_CNT, pTable1->bits[10]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE12_CNT, pTable1->bits[11]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE13_CNT, pTable1->bits[12]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE14_CNT, pTable1->bits[13]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE15_CNT, pTable1->bits[14]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE16_CNT, pTable1->bits[15]);

	/* table DC2 (the not-luma table) */

	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE1_CNT, pTable2->bits[0]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE2_CNT, pTable2->bits[1]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE3_CNT, pTable2->bits[2]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE4_CNT, pTable2->bits[3]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE5_CNT, pTable2->bits[4]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE6_CNT, pTable2->bits[5]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE7_CNT, pTable2->bits[6]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE8_CNT, pTable2->bits[7]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE9_CNT, pTable2->bits[8]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE10_CNT, pTable2->bits[9]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE11_CNT, pTable2->bits[10]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE12_CNT, pTable2->bits[11]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE13_CNT, pTable2->bits[12]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE14_CNT, pTable2->bits[13]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE15_CNT, pTable2->bits[14]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE16_CNT, pTable2->bits[15]);

	return;

#undef JPG_SCN
#undef JPG_VLC
#undef JPG_QTB
#undef JPG_FRM
#undef PTR_JPGC
}

/*------------------------------------------------------------------------------

        Function name: JpegDecWriteLenBitsNonInterleaved

        Functional description:
          tell hw how many vlc words of different lengths we have

        Inputs:
          JpegDecContainer *pJpegDecCont

        Outputs:
          void

------------------------------------------------------------------------------*/
static void JpegDecWriteLenBitsNonInterleaved(JpegDecContainer *pJpegDecCont)
{

#define PTR_JPGC    pJpegDecCont
#define JPG_SCN     pJpegDecCont->scan
#define JPG_VLC     pJpegDecCont->vlc
#define JPG_QTB     pJpegDecCont->quant
#define JPG_FRM     pJpegDecCont->frame

	VlcTable *pTable1 = NULL;
	VlcTable *pTable2 = NULL;
//#ifdef NDEBUG
	UNUSED(pTable2);
//#endif

	/* first select the table we'll use */

	/* this trick is done because hw always wants luma table as ac hw table 1 */
	if (JPG_SCN.Ta[PTR_JPGC->info.componentId] == 0) {

		pTable1 = &(JPG_VLC.acTable0);
		pTable2 = &(JPG_VLC.acTable1);

	} else {

		pTable1 = &(JPG_VLC.acTable1);
		pTable2 = &(JPG_VLC.acTable0);
	}

	ASSERT(pTable1);
	ASSERT(pTable2);

	/* write AC table 1 (luma) */

	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE1_CNT, pTable1->bits[0]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE2_CNT, pTable1->bits[1]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE3_CNT, pTable1->bits[2]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE4_CNT, pTable1->bits[3]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE5_CNT, pTable1->bits[4]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE6_CNT, pTable1->bits[5]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE7_CNT, pTable1->bits[6]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE8_CNT, pTable1->bits[7]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE9_CNT, pTable1->bits[8]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE10_CNT, pTable1->bits[9]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE11_CNT, pTable1->bits[10]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE12_CNT, pTable1->bits[11]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE13_CNT, pTable1->bits[12]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE14_CNT, pTable1->bits[13]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE15_CNT, pTable1->bits[14]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE16_CNT, pTable1->bits[15]);

	if (JPG_SCN.Td[PTR_JPGC->info.componentId] == 0) {

		pTable1 = &(JPG_VLC.dcTable0);
		pTable2 = &(JPG_VLC.dcTable1);

	} else {

		pTable1 = &(JPG_VLC.dcTable1);
		pTable2 = &(JPG_VLC.dcTable0);
	}

	ASSERT(pTable1);
	ASSERT(pTable2);

	/* write DC table 1 (luma) */
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE1_CNT, pTable1->bits[0]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE2_CNT, pTable1->bits[1]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE3_CNT, pTable1->bits[2]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE4_CNT, pTable1->bits[3]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE5_CNT, pTable1->bits[4]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE6_CNT, pTable1->bits[5]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE7_CNT, pTable1->bits[6]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE8_CNT, pTable1->bits[7]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE9_CNT, pTable1->bits[8]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE10_CNT, pTable1->bits[9]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE11_CNT, pTable1->bits[10]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE12_CNT, pTable1->bits[11]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE13_CNT, pTable1->bits[12]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE14_CNT, pTable1->bits[13]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE15_CNT, pTable1->bits[14]);
	SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE16_CNT, pTable1->bits[15]);

	return;

#undef JPG_SCN
#undef JPG_VLC
#undef JPG_QTB
#undef JPG_FRM
#undef PTR_JPGC
}

/*------------------------------------------------------------------------------

        Function name: JpegDecWriteLenBitsProgressive

        Functional description:
          tell hw how many vlc words of different lengths we have

        Inputs:
          JpegDecContainer *pJpegDecCont

        Outputs:
          void

------------------------------------------------------------------------------*/
static void JpegDecWriteLenBitsProgressive(JpegDecContainer *pJpegDecCont)
{

	u32 i;

#define PTR_JPGC    pJpegDecCont
#define JPG_SCN     pJpegDecCont->scan
#define JPG_VLC     pJpegDecCont->vlc
#define JPG_QTB     pJpegDecCont->quant
#define JPG_FRM     pJpegDecCont->frame

	VlcTable *pTable1 = NULL;
	VlcTable *pTable2 = NULL;
	VlcTable *pTable3 = NULL;

	/* reset swregs that contains vlc length information: swregs [16-28] */
	for (i = JPEGDEC_VLC_LEN_START_REG; i < JPEGDEC_VLC_LEN_END_REG; i++) {
		PTR_JPGC->jpegRegs[i] = 0;
	}

	/* check if interleaved scan ==> only one table needed */
	if (PTR_JPGC->info.nonInterleaved) {
		/* check if AC or DC coefficient scan */
		if (PTR_JPGC->scan.Ss == 0) { /* DC */
			/* check component ID */
			if (PTR_JPGC->info.componentId == 0) {
				if (JPG_SCN.Td[0] == 0) {
					pTable1 = &(JPG_VLC.dcTable0);
				} else if (JPG_SCN.Td[0] == 1) {
					pTable1 = &(JPG_VLC.dcTable1);
				} else if (JPG_SCN.Td[0] == 2) {
					pTable1 = &(JPG_VLC.dcTable2);
				} else {
					pTable1 = &(JPG_VLC.dcTable3);
				}
			} else if (PTR_JPGC->info.componentId == 1) {
				if (JPG_SCN.Td[1] == 0) {
					pTable1 = &(JPG_VLC.dcTable0);
				} else if (JPG_SCN.Td[1] == 1) {
					pTable1 = &(JPG_VLC.dcTable1);
				} else if (JPG_SCN.Td[1] == 2) {
					pTable1 = &(JPG_VLC.dcTable2);
				} else {
					pTable1 = &(JPG_VLC.dcTable3);
				}
			} else {
				if (JPG_SCN.Td[2] == 0) {
					pTable1 = &(JPG_VLC.dcTable0);
				} else if (JPG_SCN.Td[2] == 1) {
					pTable1 = &(JPG_VLC.dcTable1);
				} else if (JPG_SCN.Td[2] == 2) {
					pTable1 = &(JPG_VLC.dcTable2);
				} else {
					pTable1 = &(JPG_VLC.dcTable3);
				}
			}

			ASSERT(pTable1);

			/* if later stage DC ==> no need for table */
			if (PTR_JPGC->scan.Ah == 0) {
				/* write DC table 1 */
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE1_CNT,
							   pTable1->bits[0]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE2_CNT,
							   pTable1->bits[1]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE3_CNT,
							   pTable1->bits[2]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE4_CNT,
							   pTable1->bits[3]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE5_CNT,
							   pTable1->bits[4]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE6_CNT,
							   pTable1->bits[5]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE7_CNT,
							   pTable1->bits[6]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE8_CNT,
							   pTable1->bits[7]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE9_CNT,
							   pTable1->bits[8]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE10_CNT,
							   pTable1->bits[9]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE11_CNT,
							   pTable1->bits[10]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE12_CNT,
							   pTable1->bits[11]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE13_CNT,
							   pTable1->bits[12]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE14_CNT,
							   pTable1->bits[13]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE15_CNT,
							   pTable1->bits[14]);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE16_CNT,
							   pTable1->bits[15]);
			} else {
				/* write zero table */
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE1_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE2_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE3_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE4_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE5_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE6_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE7_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE8_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE9_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE10_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE11_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE12_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE13_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE14_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE15_CNT, 0);
				SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE16_CNT, 0);
			}

		} else { /* AC */
			/* check component ID */
			if (PTR_JPGC->info.componentId == 0) {
				if (JPG_SCN.Ta[0] == 0) {
					pTable1 = &(JPG_VLC.acTable0);
				} else if (JPG_SCN.Ta[0] == 1) {
					pTable1 = &(JPG_VLC.acTable1);
				} else if (JPG_SCN.Ta[0] == 2) {
					pTable1 = &(JPG_VLC.acTable2);
				} else {
					pTable1 = &(JPG_VLC.acTable3);
				}
			} else if (PTR_JPGC->info.componentId == 1) {
				if (JPG_SCN.Ta[1] == 0) {
					pTable1 = &(JPG_VLC.acTable0);
				} else if (JPG_SCN.Ta[1] == 1) {
					pTable1 = &(JPG_VLC.acTable1);
				} else if (JPG_SCN.Ta[1] == 2) {
					pTable1 = &(JPG_VLC.acTable2);
				} else {
					pTable1 = &(JPG_VLC.acTable3);
				}
			} else {
				if (JPG_SCN.Ta[2] == 0) {
					pTable1 = &(JPG_VLC.acTable0);
				} else if (JPG_SCN.Ta[2] == 1) {
					pTable1 = &(JPG_VLC.acTable1);
				} else if (JPG_SCN.Ta[2] == 2) {
					pTable1 = &(JPG_VLC.acTable2);
				} else {
					pTable1 = &(JPG_VLC.acTable3);
				}
			}

			ASSERT(pTable1);

			/* write AC table 1 */
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE1_CNT,
						   pTable1->bits[0]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE2_CNT,
						   pTable1->bits[1]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE3_CNT,
						   pTable1->bits[2]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE4_CNT,
						   pTable1->bits[3]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE5_CNT,
						   pTable1->bits[4]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE6_CNT,
						   pTable1->bits[5]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE7_CNT,
						   pTable1->bits[6]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE8_CNT,
						   pTable1->bits[7]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE9_CNT,
						   pTable1->bits[8]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE10_CNT,
						   pTable1->bits[9]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE11_CNT,
						   pTable1->bits[10]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE12_CNT,
						   pTable1->bits[11]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE13_CNT,
						   pTable1->bits[12]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE14_CNT,
						   pTable1->bits[13]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE15_CNT,
						   pTable1->bits[14]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_AC1_CODE16_CNT,
						   pTable1->bits[15]);
		}
	} else { /* interleaved */
		/* first select the table we'll use */
		/* this trick is done because hw always wants luma table as ac hw table 1 */

		if (JPG_SCN.Td[0] == 0) {
			pTable1 = &(JPG_VLC.dcTable0);
		} else if (JPG_SCN.Td[0] == 1) {
			pTable1 = &(JPG_VLC.dcTable1);
		} else if (JPG_SCN.Td[0] == 2) {
			pTable1 = &(JPG_VLC.dcTable2);
		} else {
			pTable1 = &(JPG_VLC.dcTable3);
		}

		if (JPG_SCN.Td[1] == 0) {
			pTable2 = &(JPG_VLC.dcTable0);
		} else if (JPG_SCN.Td[1] == 1) {
			pTable2 = &(JPG_VLC.dcTable1);
		} else if (JPG_SCN.Td[1] == 2) {
			pTable2 = &(JPG_VLC.dcTable2);
		} else {
			pTable2 = &(JPG_VLC.dcTable3);
		}

		if (JPG_SCN.Td[2] == 0) {
			pTable3 = &(JPG_VLC.dcTable0);
		} else if (JPG_SCN.Td[2] == 1) {
			pTable3 = &(JPG_VLC.dcTable1);
		} else if (JPG_SCN.Td[2] == 2) {
			pTable3 = &(JPG_VLC.dcTable2);
		} else {
			pTable3 = &(JPG_VLC.dcTable3);
		}

		ASSERT(pTable1);
		ASSERT(pTable2);
		ASSERT(pTable3);

		/* if later stage DC ==> no need for table */
		if (PTR_JPGC->scan.Ah == 0) {
			/* write DC table 1 (luma) */
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE1_CNT,
						   pTable1->bits[0]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE2_CNT,
						   pTable1->bits[1]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE3_CNT,
						   pTable1->bits[2]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE4_CNT,
						   pTable1->bits[3]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE5_CNT,
						   pTable1->bits[4]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE6_CNT,
						   pTable1->bits[5]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE7_CNT,
						   pTable1->bits[6]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE8_CNT,
						   pTable1->bits[7]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE9_CNT,
						   pTable1->bits[8]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE10_CNT,
						   pTable1->bits[9]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE11_CNT,
						   pTable1->bits[10]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE12_CNT,
						   pTable1->bits[11]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE13_CNT,
						   pTable1->bits[12]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE14_CNT,
						   pTable1->bits[13]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE15_CNT,
						   pTable1->bits[14]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE16_CNT,
						   pTable1->bits[15]);

			/* table DC2 (Cb) */
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE1_CNT,
						   pTable2->bits[0]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE2_CNT,
						   pTable2->bits[1]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE3_CNT,
						   pTable2->bits[2]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE4_CNT,
						   pTable2->bits[3]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE5_CNT,
						   pTable2->bits[4]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE6_CNT,
						   pTable2->bits[5]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE7_CNT,
						   pTable2->bits[6]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE8_CNT,
						   pTable2->bits[7]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE9_CNT,
						   pTable2->bits[8]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE10_CNT,
						   pTable2->bits[9]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE11_CNT,
						   pTable2->bits[10]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE12_CNT,
						   pTable2->bits[11]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE13_CNT,
						   pTable2->bits[12]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE14_CNT,
						   pTable2->bits[13]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE15_CNT,
						   pTable2->bits[14]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE16_CNT,
						   pTable2->bits[15]);

			/* table DC2 (Cr) */
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE1_CNT,
						   pTable3->bits[0]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE2_CNT,
						   pTable3->bits[1]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE3_CNT,
						   pTable3->bits[2]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE4_CNT,
						   pTable3->bits[3]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE5_CNT,
						   pTable3->bits[4]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE6_CNT,
						   pTable3->bits[5]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE7_CNT,
						   pTable3->bits[6]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE8_CNT,
						   pTable3->bits[7]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE9_CNT,
						   pTable3->bits[8]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE10_CNT,
						   pTable3->bits[9]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE11_CNT,
						   pTable3->bits[10]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE12_CNT,
						   pTable3->bits[11]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE13_CNT,
						   pTable3->bits[12]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE14_CNT,
						   pTable3->bits[13]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE15_CNT,
						   pTable3->bits[14]);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE16_CNT,
						   pTable3->bits[15]);
		} else {
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE1_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE2_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE3_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE4_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE5_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE6_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE7_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE8_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE9_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE10_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE11_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE12_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE13_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE14_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE15_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC1_CODE16_CNT, 0);

			/* table DC2 (Cb) */
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE1_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE2_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE3_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE4_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE5_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE6_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE7_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE8_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE9_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE10_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE11_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE12_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE13_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE14_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE15_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC2_CODE16_CNT, 0);

			/* table DC2 (Cr) */
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE1_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE2_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE3_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE4_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE5_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE6_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE7_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE8_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE9_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE10_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE11_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE12_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE13_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE14_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE15_CNT, 0);
			SetDecRegister(PTR_JPGC->jpegRegs, HWIF_DC3_CODE16_CNT, 0);
		}
	}

	return;

#undef JPG_SCN
#undef JPG_VLC
#undef JPG_QTB
#undef JPG_FRM
#undef PTR_JPGC
}

/*------------------------------------------------------------------------------

        Function name: JpegDecNextScanHdrs

        Functional description:
          Decodes next headers in case of non-interleaved stream

        Inputs:
          JpegDecContainer *pDecData      Pointer to JpegDecContainer structure

        Outputs:
          OK/NOK

------------------------------------------------------------------------------*/
JpegDecRet JpegDecNextScanHdrs(JpegDecContainer *pJpegDecCont)
{

	u32 i;
	u32 currentByte = 0;
	u32 currentBytes = 0;
	JpegDecRet retCode;

#define PTR_JPGC    pJpegDecCont
#define JPG_SCN     pJpegDecCont->scan
#define JPG_VLC     pJpegDecCont->vlc
#define JPG_QTB     pJpegDecCont->quant
#define JPG_FRM     pJpegDecCont->frame

	retCode = JPEGDEC_OK;

	/* reset for new headers */
	PTR_JPGC->image.headerReady = 0;

	/* find markers and go ! */
	do {
		/* Look for marker prefix byte from stream */
		if (JpegDecGetByte(&(PTR_JPGC->stream)) == 0xFF) {
			currentByte = JpegDecGetByte(&(PTR_JPGC->stream));

			/* switch to certain header decoding */
			switch (currentByte) {
			case 0x00:
			case SOF0:
			case SOF2:
				break;
			/* Start of Scan */
			case SOS:
				/* reset image ready */
				PTR_JPGC->image.imageReady = 0;
				retCode = JpegDecDecodeScan(PTR_JPGC);
				PTR_JPGC->image.headerReady = 1;
				if (retCode != JPEGDEC_OK) {
					if (retCode == JPEGDEC_STRM_ERROR) {
						JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# ERROR: Stream error"));
						return (retCode);
					} else {
						JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# JpegDecDecodeScan err\n"));
						return (retCode);
					}
				}

				if (PTR_JPGC->stream.bitPosInByte) {
					/* delete stuffing bits */
					currentByte = (8 - PTR_JPGC->stream.bitPosInByte);
					if (JpegDecFlushBits
						(&(PTR_JPGC->stream),
						 8 - PTR_JPGC->stream.bitPosInByte) == STRM_ERROR) {
						JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# ERROR: Stream error"));
						return (JPEGDEC_STRM_ERROR);
					}
				}
				JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# Stuffing bits deleted\n"));
				break;
			/* Start of Huffman tables */
			case DHT:
				JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# JpegDecDecodeHuffmanTables dec"));
				retCode = JpegDecDecodeHuffmanTables(PTR_JPGC);
				JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# JpegDecDecodeHuffmanTables stops"));
				if (retCode != JPEGDEC_OK) {
					if (retCode == JPEGDEC_STRM_ERROR) {
						JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# ERROR: Stream error"));
						return (retCode);
					} else {
						JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# ERROR: JpegDecDecodeHuffmanTables err"));
						return (retCode);
					}
				}
				break;
			/* start of Quantisation Tables */
			case DQT:
				JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# JpegDecDecodeQuantTables dec"));
				retCode = JpegDecDecodeQuantTables(PTR_JPGC);
				if (retCode != JPEGDEC_OK) {
					if (retCode == JPEGDEC_STRM_ERROR) {
						JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# ERROR: Stream error"));
						return (retCode);
					} else {
						JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# ERROR: JpegDecDecodeQuantTables err"));
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
					JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# EOI: OK\n"));
					return (JPEGDEC_FRAME_READY);
				} else {
					JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# ERROR: EOI: NOK\n"));
					return (JPEGDEC_ERROR);
				}
			/* Define Restart Interval */
			case DRI:
				JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# DRI"));
				currentBytes = JpegDecGet2Bytes(&(PTR_JPGC->stream));
				if (currentBytes == STRM_ERROR) {
					JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# ERROR: Read bits "));
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
				JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# DC predictors init"));
				break;
			/* unsupported features */
			case DNL:
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
				JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# ERROR: Unsupported Features"));
				return (JPEGDEC_UNSUPPORTED);
			/* application data & comments */
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
			case COM:
				JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# COM"));
				currentBytes = JpegDecGet2Bytes(&(PTR_JPGC->stream));
				if (currentBytes == STRM_ERROR) {
					JPEGDEC_TRACE_INTERNAL(("JpegDecNextScanHdrs# ERROR: Read bits "));
					return (JPEGDEC_STRM_ERROR);
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

	return (JPEGDEC_OK);

#undef JPG_SCN
#undef JPG_VLC
#undef JPG_QTB
#undef JPG_FRM
#undef PTR_JPGC
}

/*------------------------------------------------------------------------------
    Function name   : JpegRefreshRegs
    Description     :
    Return type     : void
    Argument        : PPContainer * ppC
------------------------------------------------------------------------------*/
void JpegRefreshRegs(JpegDecContainer *pJpegDecCont)
{
	i32 i;
	u32 offset = 0x0;

	u32 *ppRegs = pJpegDecCont->jpegRegs;

	for (i = DEC_X170_REGISTERS; i > 0; i--) {
		*ppRegs++ = DWLReadReg(pJpegDecCont->dwl, pJpegDecCont->coreID, offset);
		offset += 4;
	}
	ppRegs = pJpegDecCont->jpegRegs + DEC_PERF_CYCLE_COUNT;
	*ppRegs = DWLReadReg(pJpegDecCont->dwl, pJpegDecCont->coreID, DEC_PERF_CYCLE_COUNT * 4);
#ifdef USE_64BIT_ENV
#ifndef VERIFY_32BIT_NANOD_IN_64BIT_ENV
	offset = TOTAL_X170_ORIGIN_REGS * 0x04;
	ppRegs = pJpegDecCont->jpegRegs + TOTAL_X170_ORIGIN_REGS;
	for (i = DEC_X170_EXPAND_REGS; i > 0; --i) {
		*ppRegs++ = DWLReadReg(pJpegDecCont->dwl, pJpegDecCont->coreID, offset);
		offset += 4;
	}
#endif
#endif
}

/*------------------------------------------------------------------------------
    Function name   : JpegFlushRegs
    Description     :
    Return type     : void
    Argument        : PPContainer * ppC
------------------------------------------------------------------------------*/
void JpegFlushRegs(JpegDecContainer *pJpegDecCont)
{
	i32 i;
	u32 offset = 0x4;
	u32 *ppRegs = pJpegDecCont->jpegRegs;

#ifdef JPEGDEC_ASIC_TRACE
	{
		JPEGDEC_TRACE_INTERNAL(("INTERNAL: REGS BEFORE HW ENABLE\n"));
		PrintJPEGReg(pJpegDecCont->jpegRegs);
	}
#endif /* #ifdef JPEGDEC_ASIC_TRACE */

#ifdef JPEGDEC_INTEGRATOR
	DWLWriteReg(pJpegDecCont->dwl, 0, 0x00000000);
#endif /* #ifdef JPEGDEC_INTEGRATOR */

	/* skip id register */
	ppRegs++;

	for (i = DEC_X170_REGISTERS; i > 1; i--) {
		DWLWriteReg(pJpegDecCont->dwl, pJpegDecCont->coreID, offset, *ppRegs);
		*ppRegs = 0;
		ppRegs++;
		offset += 4;
	}
	DWLWriteReg(pJpegDecCont->dwl, pJpegDecCont->coreID, 77 * 4, pJpegDecCont->jpegRegs[77]);
#ifdef USE_64BIT_ENV
#ifndef VERIFY_32BIT_NANOD_IN_64BIT_ENV
	offset = TOTAL_X170_ORIGIN_REGS * 0x04;
	ppRegs = pJpegDecCont->jpegRegs + TOTAL_X170_ORIGIN_REGS;
	for (i = DEC_X170_EXPAND_REGS; i > 0; --i) {
		DWLWriteReg(pJpegDecCont->dwl, pJpegDecCont->coreID, offset, *ppRegs);
		*ppRegs = 0;
		ppRegs++;
		offset += 4;
	}
#endif
#endif
}

/*------------------------------------------------------------------------------
    Function name   : JpegDecInitHWEmptyScan
    Description     :
    Return type     : void
    Argument        :
------------------------------------------------------------------------------*/
static u32 NumBits(u32 value)
{

	u32 numBits = 0;

	while (value) {
		value >>= 1;
		numBits++;
	}

	if (!numBits) {
		numBits = 1;
	}

	return (numBits);

}

void JpegDecInitHWEmptyScan(JpegDecContainer *pJpegDecCont, u32 componentId)
{

	u32 i;
	i32 n;
	g1_addr_t coeffBuffer = 0;
	g1_addr_t outputBuffer = 0;
	u32 numBlocks;
	u32 numMax;
	u8 *pStrm;
	u32 bits;
	u32 bitPos;
	u32 *pTableBase = NULL;

	ASSERT(pJpegDecCont);

	pJpegDecCont->info.nonInterleaved = 1;
	pJpegDecCont->info.componentId = componentId;

	if (pJpegDecCont->ppInstance == NULL && pJpegDecCont->info.userAllocMem == 1) {
		/* if user allocated memory ==> new addresses */
		pJpegDecCont->asicBuff.outLumaBuffer.virtualAddress =
			pJpegDecCont->info.givenOutLuma.virtualAddress;
		pJpegDecCont->asicBuff.outLumaBuffer.busAddress =
			pJpegDecCont->info.givenOutLuma.busAddress;
		pJpegDecCont->asicBuff.outChromaBuffer.virtualAddress =
			pJpegDecCont->info.givenOutChroma.virtualAddress;
		pJpegDecCont->asicBuff.outChromaBuffer.busAddress =
			pJpegDecCont->info.givenOutChroma.busAddress;
		pJpegDecCont->asicBuff.outChromaBuffer2.virtualAddress =
			pJpegDecCont->info.givenOutChroma2.virtualAddress;
		pJpegDecCont->asicBuff.outChromaBuffer2.busAddress =
			pJpegDecCont->info.givenOutChroma2.busAddress;
	}

	/*************** Set swreg13 data ************/
	/* Luminance output */
	if (componentId == 0) {
		outputBuffer = pJpegDecCont->asicBuff.outLumaBuffer.busAddress;
	} else if (componentId == 1) {
		outputBuffer = (pJpegDecCont->asicBuff.outChromaBuffer.busAddress);
	} else {
		outputBuffer = (pJpegDecCont->asicBuff.outChromaBuffer2.busAddress);
	}

	SET_ADDR_REG(pJpegDecCont->jpegRegs, HWIF_DEC_OUT_BASE, outputBuffer);

	pJpegDecCont->info.yCbCrMode = 0;
	pJpegDecCont->info.X = pJpegDecCont->frame.hwX;
	pJpegDecCont->info.Y = pJpegDecCont->frame.hwY;
	pJpegDecCont->info.fillX = 0;
	pJpegDecCont->info.fillY = 0;
	numBlocks = pJpegDecCont->frame.hwX * pJpegDecCont->frame.hwY / 64;
	coeffBuffer = pJpegDecCont->info.pCoeffBase.busAddress;
	if (componentId) {
		coeffBuffer += JPEGDEC_COEFF_SIZE * numBlocks;
		if (pJpegDecCont->info.yCbCrModeOrig == JPEGDEC_YUV420) {
			pJpegDecCont->info.X /= 2;
			if (pJpegDecCont->info.X & 0xF) {
				pJpegDecCont->info.X += 8;
				pJpegDecCont->info.fillX = 1;
			}
			pJpegDecCont->info.Y /= 2;
			if (pJpegDecCont->info.Y & 0xF) {
				pJpegDecCont->info.Y += 8;
				pJpegDecCont->info.fillY = 1;
			}
			numBlocks /= 4;
		} else if (pJpegDecCont->info.yCbCrModeOrig == JPEGDEC_YUV422) {
			pJpegDecCont->info.X /= 2;
			if (pJpegDecCont->info.X & 0xF) {
				pJpegDecCont->info.X += 8;
				pJpegDecCont->info.fillX = 1;
			}
			numBlocks /= 2;
		} else if (pJpegDecCont->info.yCbCrModeOrig == JPEGDEC_YUV440) {
			pJpegDecCont->info.Y /= 2;
			if (pJpegDecCont->info.Y & 0xF) {
				pJpegDecCont->info.Y += 8;
				pJpegDecCont->info.fillY = 1;
			}
			numBlocks /= 2;
		}
		if (componentId > 1) {
			coeffBuffer += JPEGDEC_COEFF_SIZE * numBlocks;
		}
	}

	pStrm = (u8 *)pJpegDecCont->info.tmpStrm.virtualAddress;
	numMax = 0;
	while (numBlocks > 32767) {
		numBlocks -= 32767;
		numMax++;
	}

	n = NumBits(numBlocks);

	/* do we still have correct quantization tables ?? */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Write AC,DC,QP tables to base\n"));
	JpegDecWriteTablesProgressive(pJpegDecCont);

	/* two vlc codes, both with length 1 (can be done?), 0 for largest eob, 1
	 * for last eob (EOBn) */
	/* write "length amounts" */
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_AC1_CODE1_CNT, 2);

	/* codeword values 0xE0 (for EOB run of 32767 blocks) and 0xn0 */
	pTableBase = pJpegDecCont->frame.pTableBase.virtualAddress;
	pTableBase += 48;   /* start of vlc tables */
	*pTableBase = (((u32)0xE0) << 24) | ((n - 1) << 20);

	/* write numMax ext eobs of length 32767 followed by last ext eob */
	bitPos = 0;
	for (i = 0; i < numMax; i++) {
		bits = 0x3FFF << 17;
		*pStrm = (bitPos ? *pStrm : 0) | bits >> (24 + bitPos);
		pStrm++;
		bits <<= 8 - bitPos;
		*pStrm = bits >> 24;
		if (bitPos >= 1) {
			pStrm++;
			bits <<= 8;
			*pStrm = bits >> 24;
		}
		bitPos = (bitPos + 15) & 0x7;
	}

	if (numBlocks) {
		/* codeword to be written:
		 * '1' to indicate EOBn followed by number of blocks - 2^(n-1) */
		bits = numBlocks << (32 - n);
		*pStrm = (bitPos ? *pStrm : 0) | bits >> (24 + bitPos);
		pStrm++;
		bits <<= 8 - bitPos;
		n -= 8 - bitPos;
		while (n > 0) {
			*pStrm++ = bits >> 24;
			bits <<= 8;
			n -= 8;
		}
	}

	SET_ADDR_REG(pJpegDecCont->jpegRegs, HWIF_RLC_VLC_BASE,
				 pJpegDecCont->info.tmpStrm.busAddress);
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_STRM_START_BIT, 0);
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_STREAM_LEN, 100);

	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set input format\n"));
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_JPEG_MODE, JPEGDEC_YUV400);

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame width extension\n"));
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_PIC_MB_W_EXT,
				   ((((pJpegDecCont->info.X) >> (4)) & 0xE00) >> 9));

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame width\n"));
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_PIC_MB_WIDTH,
				   ((pJpegDecCont->info.X) >> (4)) & 0x1FF);

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame height extension\n"));
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_PIC_MB_H_EXT,
				   ((((pJpegDecCont->info.Y) >> (4)) & 0x700) >> 8));

	/* frame size, round up the number of mbs */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set Frame height\n"));
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_PIC_MB_HEIGHT_P,
				   ((pJpegDecCont->info.Y) >> (4)) & 0x0FF);

	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_PJPEG_WDIV8,
				   pJpegDecCont->info.fillX);
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_JPEG_FILRIGHT_E,
				   pJpegDecCont->info.fillX);
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_PJPEG_HDIV8,
				   pJpegDecCont->info.fillY);
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_PJPEG_FILDOWN_E,
				   pJpegDecCont->info.fillY);

	/*************** Set swreg52 data ************/
	/* Set JPEG operation mode */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set JPEG operation mode\n"));
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_PJPEG_E, 1);

	/* indicate first ac scan for any spectral coeffs, nothing will be changed
	 * as every block "skipped" by extended eobs */
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_PJPEG_SS, 1);
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_PJPEG_SE, 1);
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_PJPEG_AH, 0);
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_PJPEG_AL, 0);

	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set coefficient buffer base address\n"));
	SET_ADDR_REG(pJpegDecCont->jpegRegs, HWIF_PJPEG_COEFF_BUF, coeffBuffer);
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_DEC_OUT_DIS, 0);

	/* write table base */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Set AC,DC,QP table base address\n"));
	SET_ADDR_REG(pJpegDecCont->jpegRegs, HWIF_QTABLE_BASE,
				 pJpegDecCont->frame.pTableBase.busAddress);

	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_SYNC_MARKER_E, 0);

	pJpegDecCont->asicRunning = 1;

	/* Flush regs to hw register */
	JpegFlushRegs(pJpegDecCont);
	DWLWriteReg(pJpegDecCont->dwl, pJpegDecCont->coreID, 4 * 101, pJpegDecCont->jpegRegs[101]);

	/* Enable jpeg mode and set slice mode */
	JPEGDEC_TRACE_INTERNAL(("INTERNAL: Enable jpeg\n"));
	SetDecRegister(pJpegDecCont->jpegRegs, HWIF_DEC_E, 1);
	DWLEnableHW(pJpegDecCont->dwl, pJpegDecCont->coreID, 4 * 1,
				pJpegDecCont->jpegRegs[1]);

#ifdef JPEGDEC_ASIC_TRACE
	{
		JPEGDEC_TRACE_INTERNAL(("PROGRESSIVE CONTINUE: REGS BEFORE IRQ CLEAN\n"));
		PrintJPEGReg(pJpegDecCont->jpegRegs);
	}
#endif /* #ifdef JPEGDEC_ASIC_TRACE */

}
