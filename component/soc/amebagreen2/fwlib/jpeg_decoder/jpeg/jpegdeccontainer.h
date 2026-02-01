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

#ifndef JPEGDECCONT_H
#define JPEGDECCONT_H

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/
#include "basetype.h"
#include "jpegdecapi.h"
#include "dwl.h"
#include "deccfg.h"
#include "decppif.h"

#ifdef _ASSERT_USED
#include <assert.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
    2. Module defines
------------------------------------------------------------------------------*/

/* macro for assertion, used only if compiler flag _ASSERT_USED is defined */
#ifdef _ASSERT_USED
#define ASSERT(expr) assert(expr)
#else
#define ASSERT(expr)
#endif

#define MIN_NUMBER_OF_COMPONENTS 1
#define MAX_NUMBER_OF_COMPONENTS 3

#define JPEGDEC_X170_MIN_BUFFER 5120
#define JPEGDEC_X170_MAX_BUFFER 16776960
#define JPEGDEC_MAX_SLICE_SIZE 4096
#define JPEGDEC_TABLE_SIZE 544
#define JPEGDEC_MIN_WIDTH 48
#define JPEGDEC_MIN_HEIGHT 48
#define JPEGDEC_MAX_WIDTH 4672
#define JPEGDEC_MAX_HEIGHT 4672
#define JPEGDEC_MAX_PIXEL_AMOUNT 16370688
#define JPEGDEC_MAX_WIDTH_8190 16384
#define JPEGDEC_MAX_HEIGHT_8190 16384
#define JPEGDEC_MAX_PIXEL_AMOUNT_8190 268435456
#define JPEGDEC_MAX_SLICE_SIZE_8190 8100
#define JPEGDEC_MAX_WIDTH_WEBP 16384
#define JPEGDEC_MAX_HEIGHT_WEBP 16384
#define JPEGDEC_MAX_PIXEL_AMOUNT_WEBP 268435456
#define JPEGDEC_MAX_SLICE_SIZE_WEBP (1<<30)
#define JPEGDEC_MAX_WIDTH_TN 256
#define JPEGDEC_MAX_HEIGHT_TN 256
#define JPEGDEC_YUV400 0
#define JPEGDEC_YUV420 2
#define JPEGDEC_YUV422 3
#define JPEGDEC_YUV444 4
#define JPEGDEC_YUV440 5
#define JPEGDEC_YUV411 6
#define JPEGDEC_BASELINE_TABLE_SIZE 544
#define JPEGDEC_PROGRESSIVE_TABLE_SIZE 576
#define JPEGDEC_QP_BASE 32
#define JPEGDEC_AC1_BASE 48
#define JPEGDEC_AC2_BASE 88
#define JPEGDEC_DC1_BASE 129
#define JPEGDEC_DC2_BASE 132
#define JPEGDEC_DC3_BASE 135

/* progressive */
#define JPEGDEC_COEFF_SIZE 96

/*------------------------------------------------------------------------------
    3. Data types
------------------------------------------------------------------------------*/

typedef struct {
	u32 C;  /* Component id */
	u32 H;  /* Horizontal sampling factor */
	u32 V;  /* Vertical sampling factor */
	u32 Tq; /* Quantization table destination selector */
} Components;

typedef struct {
	u8 *pStartOfStream;
	u8 *pCurrPos;
	g1_addr_t streamBus;
	u32 bitPosInByte;
	u32 streamLength;
	u32 readBits;
	u32 appnFlag;
	u32 thumbnail;
	u32 returnSosMarker;
} StreamStorage;

typedef struct {
	u8 *pStartOfImage;
	u8 *pLum;
	u8 *pCr;
	u8 *pCb;
	u32 imageReady;
	u32 headerReady;
	u32 size;
	u32 sizeLuma;
	u32 sizeChroma;
	u32 ready;
	u32 columns[MAX_NUMBER_OF_COMPONENTS];
	u32 pixelsPerRow[MAX_NUMBER_OF_COMPONENTS];
} ImageData;

typedef struct {
	u32 Lf;
	u32 P;
	u32 Y;
	u32 hwY;
	u32 X;
	u32 hwX;
	u32 hwYTn;
	u32 hwXTn;
	u32 fullX;
	u32 fullY;
	u32 Nf; /* Number of components in frame */
	u32 codingType;
	u32 numMcuInFrame;
	u32 numMcuInRow;
	u32 mcuNumber;
	u32 nextRstNumber;
	g1_addr_t Ri;
	u32 driPeriod;
	u32 block;
	u32 row;
	u32 col;
	u32 cIndex;
	u32 *pBuffer;
	g1_addr_t bufferBus;
	i32 *pBufferCb;
	i32 *pBufferCr;
	DWLLinearMem_t pTableBase;
	u32 numBlocks[MAX_NUMBER_OF_COMPONENTS];
	u32 blocksPerRow[MAX_NUMBER_OF_COMPONENTS];
	u32 useAcOffset[MAX_NUMBER_OF_COMPONENTS];
	Components component[MAX_NUMBER_OF_COMPONENTS];
} FrameInfo;

typedef struct {
	u32 Ls;
	u32 Ns;
	u32 Cs[MAX_NUMBER_OF_COMPONENTS];   /* Scan component selector */
	u32 Td[MAX_NUMBER_OF_COMPONENTS];   /* Selects table for DC */
	u32 Ta[MAX_NUMBER_OF_COMPONENTS];   /* Selects table for AC */
	u32 Ss;
	u32 Se;
	u32 Ah;
	u32 Al;
	u32 index;
	i32 numIdctRows;
	i32 pred[MAX_NUMBER_OF_COMPONENTS];
} ScanInfo;

typedef struct {
	u32 sliceHeight;
	u32 amountOfQTables;
	u32 yCbCrMode;
	u32 yCbCr422;
	u32 column;
	u32 X;
	u32 Y;
	u32 memSize;
	u32 SliceCount;
	u32 SliceReadyForPause;
	u32 SliceMBCutValue;
	u32 pipeline;
	u32 userAllocMem;
	u32 sliceMbSetValue;
	u32 timeout;
	u32 rlcMode;
	u32 lumaPos;
	u32 chromaPos;
	u32 sliceStartCount;
	u32 amountOfSlices;
	u32 noSliceIrqForUser;
	u32 sliceLimitReached;
	u32 inputBufferEmpty;
	u32 fillRight;
	u32 fillBottom;
	u32 streamEnd;
	u32 streamEndFlag;
	u32 inputBufferLen;
	u32 inputStreaming;
	u32 decodedStreamLen;
	u32 init;
	u32 initThumb;
	u32 initBufferSize;
	i32 dcRes[MAX_NUMBER_OF_COMPONENTS];
	DWLLinearMem_t outLuma;
	DWLLinearMem_t outChroma;
	DWLLinearMem_t outChroma2;
	DWLLinearMem_t givenOutLuma;
	DWLLinearMem_t givenOutChroma;
	DWLLinearMem_t givenOutChroma2;
	i32 pred[MAX_NUMBER_OF_COMPONENTS];
	/* progressive parameters */
	u32 nonInterleaved;
	u32 componentId;
	u32 operationType;
	u32 operationTypeThumb;
	u32 progressiveScanReady;
	u32 nonInterleavedScanReady;
	u32 allocated;
	u32 yCbCrModeOrig;
	u32 getInfoYCbCrMode;
	u32 getInfoYCbCrModeTn;
	u32 components[MAX_NUMBER_OF_COMPONENTS];
	DWLLinearMem_t pCoeffBase;

	u32 fillX;
	u32 fillY;

	u32 progressiveFinish;
	u32 pfCompId;
	u32 pfNeeded[MAX_NUMBER_OF_COMPONENTS];
	DWLLinearMem_t tmpStrm;

} DecInfo;

typedef struct {

	DWLLinearMem_t outLumaBuffer;
	DWLLinearMem_t outChromaBuffer;
	DWLLinearMem_t outChromaBuffer2;

} JpegAsicBuffers;

typedef struct {
	u32 bits[16];
	u32 *vals;
	u32 tableLength;
	u32 start;
	u32 last;
} VlcTable;

typedef struct {
	u32 Lh;
	u32 defaultTables;
	VlcTable acTable0;
	VlcTable acTable1;
	VlcTable acTable2;
	VlcTable acTable3;
	VlcTable dcTable0;
	VlcTable dcTable1;
	VlcTable dcTable2;
	VlcTable dcTable3;
	VlcTable *table;
} HuffmanTables;

typedef struct {
	u32 Lq; /* Quantization table definition length */
	u32 table0[64];
	u32 table1[64];
	u32 table2[64];
	u32 table3[64];
	u32 *table;
} QuantTables;

typedef struct {
	u32 jpegRegs[TOTAL_X170_REGISTERS];
	u32 asicRunning;
	StreamStorage stream;
	FrameInfo frame;
	ImageData image;
	ScanInfo scan;
	DecInfo info;
	HuffmanTables vlc;
	QuantTables quant;
	u32 tmpData[64];
	u32 is8190;
	u32 fuseBurned;
	u32 minSupportedWidth;
	u32 minSupportedHeight;
	u32 maxSupportedWidth;
	u32 maxSupportedHeight;
	u32 maxSupportedPixelAmount;
	u32 maxSupportedSliceSize;
	u32 extensionsSupported;
	JpegAsicBuffers asicBuff;
	DecPpInterface ppControl;
	DecPpQuery ppConfigQuery;   /* Decoder asks pp info about setup, info stored here */
	u32 ppStatus;
	const void *dwl;    /* DWL instance */
	i32 coreID;

	const void *ppInstance;
	void (*PPRun)(const void *, const DecPpInterface *);
	void (*PPEndCallback)(const void *);
	void (*PPConfigQuery)(const void *, DecPpQuery *);
	u32 DHTfromStream;
} JpegDecContainer;

/*------------------------------------------------------------------------------
    4. Function prototypes
------------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* #endif JPEGDECDATA_H */
