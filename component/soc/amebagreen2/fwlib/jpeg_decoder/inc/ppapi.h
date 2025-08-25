/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PPAPI_H__
#define __PPAPI_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "basetype.h"
#include "decapicommon.h"

#define PP_JPEGDEC_PIPELINE_SUPPORT                     1

#define PP_PIPELINE_DISABLED                            0U
#define PP_PIPELINED_DEC_TYPE_H264                      1U
#define PP_PIPELINED_DEC_TYPE_MPEG4                     2U
#define PP_PIPELINED_DEC_TYPE_JPEG                      3U
#define PP_PIPELINED_DEC_TYPE_VC1                       4U
#define PP_PIPELINED_DEC_TYPE_MPEG2                     5U
#define PP_PIPELINED_DEC_TYPE_VP6                       6U
#define PP_PIPELINED_DEC_TYPE_AVS                       7U
#define PP_PIPELINED_DEC_TYPE_RV                        8U
#define PP_PIPELINED_DEC_TYPE_VP8                       9U
#define PP_PIPELINED_DEC_TYPE_WEBP                     10U

#define PP_PIX_FMT_YCBCR_4_0_0                          0x080000U

#define PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED              0x010001U
#define PP_PIX_FMT_YCRYCB_4_2_2_INTERLEAVED             0x010005U
#define PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED             0x010006U
#define PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED             0x010007U
#define PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR               0x010002U

#define PP_PIX_FMT_YCBCR_4_2_2_TILED_4X4                0x010008U
#define PP_PIX_FMT_YCRYCB_4_2_2_TILED_4X4               0x010009U
#define PP_PIX_FMT_CBYCRY_4_2_2_TILED_4X4               0x01000AU
#define PP_PIX_FMT_CRYCBY_4_2_2_TILED_4X4               0x01000BU

#define PP_PIX_FMT_YCBCR_4_4_0                          0x010004U

#define PP_PIX_FMT_YCBCR_4_2_0_PLANAR                   0x020000U
#define PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR               0x020001U
#define PP_PIX_FMT_YCBCR_4_2_0_TILED                    0x020002U

#define PP_PIX_FMT_YCBCR_4_1_1_SEMIPLANAR               0x100001U
#define PP_PIX_FMT_YCBCR_4_4_4_SEMIPLANAR               0x200001U

#define PP_PIX_FMT_RGB16_CUSTOM                         0x040000U
#define PP_PIX_FMT_RGB16_5_5_5                          0x040001U
#define PP_PIX_FMT_RGB16_5_6_5                          0x040002U
#define PP_PIX_FMT_BGR16_5_5_5                          0x040003U
#define PP_PIX_FMT_BGR16_5_6_5                          0x040004U

#define PP_PIX_FMT_RGB32_CUSTOM                         0x041000U
#define PP_PIX_FMT_RGB32                                0x041001U
#define PP_PIX_FMT_BGR32                                0x041002U

#define PP_YCBCR2RGB_TRANSFORM_CUSTOM                   0U
#define PP_YCBCR2RGB_TRANSFORM_BT_601                   1U
#define PP_YCBCR2RGB_TRANSFORM_BT_709                   2U

#define PP_ROTATION_NONE                                0U
#define PP_ROTATION_RIGHT_90                            1U
#define PP_ROTATION_LEFT_90                             2U
#define PP_ROTATION_HOR_FLIP                            3U
#define PP_ROTATION_VER_FLIP                            4U
#define PP_ROTATION_180                                 5U

#define PP_PIC_FRAME_OR_TOP_FIELD                       0U
#define PP_PIC_BOT_FIELD                                1U
#define PP_PIC_TOP_AND_BOT_FIELD                        2U
#define PP_PIC_TOP_AND_BOT_FIELD_FRAME                  3U
#define PP_PIC_TOP_FIELD_FRAME                          4U
#define PP_PIC_BOT_FIELD_FRAME                          5U

#define PP_MAX_MULTIBUFFER                              17

typedef const void *PPInst;

typedef enum PPResult_ {
	PP_OK = 0,
	PP_PARAM_ERROR = -1,
	PP_MEMFAIL = -4,

	PP_SET_IN_SIZE_INVALID = -64,
	PP_SET_IN_ADDRESS_INVALID = -65,
	PP_SET_IN_FORMAT_INVALID = -66,
	PP_SET_CROP_INVALID = -67,
	PP_SET_ROTATION_INVALID = -68,
	PP_SET_OUT_SIZE_INVALID = -69,
	PP_SET_OUT_ADDRESS_INVALID = -70,
	PP_SET_OUT_FORMAT_INVALID = -71,
	PP_SET_VIDEO_ADJUST_INVALID = -72,
	PP_SET_RGB_BITMASK_INVALID = -73,
	PP_SET_FRAMEBUFFER_INVALID = -74,
	PP_SET_MASK1_INVALID = -75,
	PP_SET_MASK2_INVALID = -76,
	PP_SET_DEINTERLACE_INVALID = -77,
	PP_SET_IN_STRUCT_INVALID = -78,
	PP_SET_IN_RANGE_MAP_INVALID = -79,
	PP_SET_ABLEND_UNSUPPORTED = -80,
	PP_SET_DEINTERLACING_UNSUPPORTED = -81,
	PP_SET_DITHERING_UNSUPPORTED = -82,
	PP_SET_SCALING_UNSUPPORTED = -83,

	PP_BUSY = -128,
	PP_HW_BUS_ERROR = -256,
	PP_HW_TIMEOUT = -257,
	PP_DWL_ERROR = -258,
	PP_SYSTEM_ERROR = -259,
	PP_DEC_COMBINED_MODE_ERROR = -512,
	PP_DEC_RUNTIME_ERROR = -513
} PPResult;

typedef struct PPInImage_ {
	u32 pixFormat;
	u32 picStruct;
	u32 videoRange;
	u32 width;
	u32 height;
	g1_addr_t bufferBusAddr;
	g1_addr_t bufferCbBusAddr;
	g1_addr_t bufferCrBusAddr;
	g1_addr_t bufferBusAddrBot;
	g1_addr_t bufferBusAddrChBot;
	u32 vc1AdvancedProfile;
	u32 vc1MultiResEnable;
	u32 vc1RangeRedFrm;
	u32 vc1RangeMapYEnable;
	u32 vc1RangeMapYCoeff;
	u32 vc1RangeMapCEnable;
	u32 vc1RangeMapCCoeff;
} PPInImage;

typedef struct PPOutImage_ {
	u32 pixFormat;
	u32 width;
	u32 height;
	g1_addr_t bufferBusAddr;
	g1_addr_t bufferChromaBusAddr;
	u32 multiBuffer;
} PPOutImage;

typedef struct PPRgbTransform_ {
	u32 a;
	u32 b;
	u32 c;
	u32 d;
	u32 e;
} PPRgbTransform;

typedef struct PPRgbBitmask_ {
	u32 maskR;
	u32 maskG;
	u32 maskB;
	u32 maskAlpha;
} PPRgbBitmask;

typedef struct PPOutRgb_ {
	u32 rgbTransform;
	i32 contrast;
	i32 brightness;
	i32 saturation;
	u32 alpha;
	u32 transparency;
	PPRgbTransform rgbTransformCoeffs;
	PPRgbBitmask rgbBitmask;
	u32 ditheringEnable;
} PPOutRgb;

typedef struct PPInCropping_ {
	u32 enable;
	/* NOTE: these are coordinates relative to the input picture */
	u32 originX;
	u32 originY;
	u32 height;
	u32 width;
} PPInCropping;

typedef struct PPOutMask1_ {
	u32 enable;
	/* NOTE: these are coordinates relative to the output image */
	i32 originX;
	i32 originY;
	u32 height;
	u32 width;
	u32 alphaBlendEna;
	g1_addr_t blendComponentBase;
	/* following parameters are to be set nonzero *only* if HW
	 * supports alpha blend cropping. */
	i32 blendOriginX;
	i32 blendOriginY;
	u32 blendWidth;
	u32 blendHeight;
} PPOutMask1;

typedef struct PPOutMask2_ {
	u32 enable;
	/* NOTE: these are coordinates relative to the output image */
	i32 originX;
	i32 originY;
	u32 height;
	u32 width;
	u32 alphaBlendEna;
	g1_addr_t blendComponentBase;
	/* following parameters are to be set nonzero *only* if HW
	 * supports alpha blend cropping. */
	i32 blendOriginX;
	i32 blendOriginY;
	u32 blendWidth;
	u32 blendHeight;
} PPOutMask2;

typedef struct PPOutFrameBuffer_ {
	u32 enable;
	/* NOTE: these are coordinates relative to the framebuffer */
	i32 writeOriginX;
	i32 writeOriginY;
	u32 frameBufferWidth;
	u32 frameBufferHeight;
} PPOutFrameBuffer;

typedef struct PPInRotation_ {
	u32 rotation;
} PPInRotation;

typedef struct PPOutDeinterlace_ {
	u32 enable;
} PPOutDeinterlace;

typedef struct PPMaxSupportedOutRes_ {
	u32 maxOutWidth;
	u32 maxOutHeight;
} PPMaxSupportedOutRes;

typedef struct PPConfig_ {
	PPInImage ppInImg;
	PPInCropping ppInCrop;
	PPInRotation ppInRotation;
	PPOutImage ppOutImg;
	PPOutRgb ppOutRgb;
	PPOutMask1 ppOutMask1;
	PPOutMask2 ppOutMask2;
	PPOutFrameBuffer ppOutFrmBuffer;
	PPOutDeinterlace ppOutDeinterlace;
	PPMaxSupportedOutRes ppMaxSupportedOutRes;
} PPConfig;

/* Version information */
typedef struct PPApiVersion_ {
	u32 major;           /* PP API major version */
	u32 minor;           /* PP API minor version */
} PPApiVersion;

typedef struct DecSwHwBuild_  PPBuild;

typedef struct PPOutput_ {
	g1_addr_t bufferBusAddr;
	u32 *virtualAddress;
	g1_addr_t bufferChromaBusAddr;
	u32 *ChromaVirtualAddress;
	u32 BufUsed;
	u32 MemIdx;
} PPOutput;

typedef struct PPOutputBuffers_ {
	u32 nbrOfBuffers;
	PPOutput ppOutputBuffers[PP_MAX_MULTIBUFFER];
} PPOutputBuffers;

/*------------------------------------------------------------------------------
    Prototypes of PP API functions
------------------------------------------------------------------------------*/
PPResult PPInit(PPInst *pPostPInst);

PPResult PPDecCombinedModeEnable(PPInst postPInst, const void *pDecInst,
								 u32 decType);

PPResult PPDecCombinedModeDisable(PPInst postPInst, const void *pDecInst);

PPResult PPGetConfig(PPInst postPInst, PPConfig *pPpConf);

PPResult PPSetConfig(PPInst postPInst, PPConfig *pPpConf);

PPResult PPDecSetMultipleOutput(PPInst postPInst,
								const PPOutputBuffers *pBuffers);

PPResult PPDecSwapLastOutputBuffer(PPInst postPInst,
								   PPOutput *pOldBuffer,
								   PPOutput *pNewBuffer);

PPResult PPGetNextOutput(PPInst postPInst, PPOutput *pOut);

void PPRelease(PPInst pPpInst);

PPResult PPGetResult(PPInst postPInst);

PPApiVersion PPGetAPIVersion(void);

PPBuild PPGetBuild(void);

void PPTrace(const char *string);

#ifdef __cplusplus
}
#endif

#endif                       /* __PPAPI_H__ */
