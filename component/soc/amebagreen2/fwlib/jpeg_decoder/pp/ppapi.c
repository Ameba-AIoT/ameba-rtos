/*------------------------------------------------------------------------------
--           Copyright (c) 2015, VeriSilicon Inc. All rights reserved              --
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
    1. Include headers
------------------------------------------------------------------------------*/
#include "vcnd_version.h"
#include "basetype.h"
#include "ppapi.h"
#include "ppinternal.h"
#include "regdrv.h"
#include "dwl.h"
#include "decppif.h"
#include "ppcfg.h"

#include "ppdebug.h"

/*------------------------------------------------------------------------------
       Version Information
------------------------------------------------------------------------------*/

#define PP_MAJOR_VERSION VCNDSW_VERSION_MAJOR
#define PP_MINOR_VERSION VCNDSW_VERSION_MINOR

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/

#ifdef PP_TRACE
#define PP_API_TRC(str)    PPTrace((str))
#else
#define PP_API_TRC(str)
#endif

#ifdef PP_H264DEC_PIPELINE_SUPPORT
#include "h264_pp_pipeline.h"
#endif

#ifdef PP_MPEG4DEC_PIPELINE_SUPPORT
#include "mpeg4_pp_pipeline.h"
#endif

#ifdef PP_JPEGDEC_PIPELINE_SUPPORT
#include "jpeg_pp_pipeline.h"
#endif

#ifdef PP_VC1DEC_PIPELINE_SUPPORT
#include "vc1hwd_pp_pipeline.h"
#endif

#ifdef PP_MPEG2DEC_PIPELINE_SUPPORT
#include "mpeg2hwd_pp_pipeline.h"
#endif

#ifdef PP_RVDEC_PIPELINE_SUPPORT
#include "rv_pp_pipeline.h"
#endif

#ifdef PP_VP6DEC_PIPELINE_SUPPORT
#include "vp6_pp_pipeline.h"
#endif

#ifdef PP_VP8DEC_PIPELINE_SUPPORT
#include "vp8hwd_pp_pipeline.h"
#endif

#ifdef PP_AVSDEC_PIPELINE_SUPPORT
#include "avs_pp_pipeline.h"
#endif

#if defined(PP_JPEGDEC_PIPELINE_SUPPORT) ||\
    defined(PP_MPEG4DEC_PIPELINE_SUPPORT) ||\
    defined(PP_H264DEC_PIPELINE_SUPPORT) ||\
    defined(PP_VC1DEC_PIPELINE_SUPPORT) ||\
    defined(PP_MPEG2DEC_PIPELINE_SUPPORT) ||\
    defined(PP_RVDEC_PIPELINE_SUPPORT) ||\
    defined(PP_AVSDEC_PIPELINE_SUPPORT) ||\
    defined(PP_VP6DEC_PIPELINE_SUPPORT) ||\
    defined(PP_VP8DEC_PIPELINE_SUPPORT)
#define PP_DEC_PIPELINE_SUPPORT
static void PPDecStartPp(const void *postPInst, const DecPpInterface *);
static void PPDecEndCallback(const void *postPInst);
static void PPDecConfigQueryFromDec(const void *postPInst, DecPpQuery *);
static void PPDecSetUpDeinterlace(PPContainer *ppC,
								  const DecPpInterface *decpp);
static void PPDecSetOutBuffer(PPContainer *ppC, const DecPpInterface *decpp);
//static u32 PPReturnMultiBufferId(const void *postPInst, g1_addr_t bus_addr);
//static u32 PPGetFreeMultiBufferId(const void *postPInst, u32 mem_id);
//static u32 PPFindDisplayId(const void *postPInst, u32 mem_id, u32 is_release);
//static void PPRecoverMemInfo(const void *postPInst, u32 idx, u32 mem_id);
//static void PPResetMultiBuffer(const void *postPInst);
//static void PPAddMultiBuffer(const void *postPInst, const void *mem_info);
#else
#undef PP_DEC_PIPELINE_SUPPORT
#endif
#if defined(PP_MPEG4DEC_PIPELINE_SUPPORT) ||\
    defined(PP_H264DEC_PIPELINE_SUPPORT) ||\
    defined(PP_VC1DEC_PIPELINE_SUPPORT) ||\
    defined(PP_MPEG2DEC_PIPELINE_SUPPORT) ||\
    defined(PP_RVDEC_PIPELINE_SUPPORT) ||\
    defined(PP_AVSDEC_PIPELINE_SUPPORT) ||\
    defined(PP_VP8DEC_PIPELINE_SUPPORT) ||\
    defined(PP_WEBPDEC_PIPELINE_SUPPORT)
static void PPDecDisplayIndex(const void *postPInst, u32 index);
#endif
#if defined(PP_MPEG4DEC_PIPELINE_SUPPORT) ||\
    defined(PP_VC1DEC_PIPELINE_SUPPORT) ||\
    defined(PP_MPEG2DEC_PIPELINE_SUPPORT) ||\
    defined(PP_RVDEC_PIPELINE_SUPPORT) ||\
    defined(PP_AVSDEC_PIPELINE_SUPPORT) ||\
    defined(PP_VP8DEC_PIPELINE_SUPPORT) ||\
    defined(PP_WEBPDEC_PIPELINE_SUPPORT)
static void PPDecBufferData(const void *postPInst, u32 bufferIndex,
							g1_addr_t inputBusLuma, g1_addr_t inputBusChroma,
							g1_addr_t bottomBusLuma, g1_addr_t bottomBusChroma);
#endif

#define PP_IS_JOINED(ppC) (ppC->decInst != NULL ? 1 : 0)

/*------------------------------------------------------------------------------
    Function name   : PPInit
    Description     : initialize pp
    Return type     : PPResult
    Argument        : PPInst * postPInst
------------------------------------------------------------------------------*/
PPResult PPInit(PPInst *pPostPInst)
{
	const void *dwl;
	PPContainer *pPpCont;
	DWLInitParam_t dwlInit;
	u32 hwId, product;
	DWLHwConfig_t hwConfig;

	PP_API_TRC("PPInit #");
	if (pPostPInst == NULL) {
		PP_API_TRC("PPInit# ERROR: PPInst == NULL");
		return (PP_PARAM_ERROR);
	}

	*pPostPInst = NULL; /* return NULL instance for any error */

	DWLReadAsicConfig(&hwConfig);

#ifndef VERIFY_32BIT_NANOD_IN_64BIT_ENV
	if (!hwConfig.addr64Support && sizeof(void *) == 8) {
		PP_API_TRC("PPInit# ERROR: HW not support 64bit address!\n");
		return (PP_PARAM_ERROR);
	}
#endif

	/* check that we have correct HW */
	hwId = DWLReadAsicID();
	product = hwId >> 16;

	PPDEBUG_PRINT(("Product %x\n", product));

	if (product < 0x8170 &&
		(product != 0x6731 && product != 0x6e64)) {
		PP_API_TRC("PPInit# ERROR: Unknown hardware");
		return PP_DWL_ERROR;
	}

	dwlInit.clientType = DWL_CLIENT_TYPE_PP;

	dwl = DWLInit(&dwlInit);

	if (dwl == NULL) {
		PP_API_TRC("PPInit# ERROR: DWL Init failed");
		return (PP_DWL_ERROR);
	}

	pPpCont = (PPContainer *) DWLmalloc(sizeof(PPContainer));

	if (pPpCont == NULL) {
		PP_API_TRC("PPInit# ERROR: Memory allocation failed");
		(void) DWLRelease(dwl);
		return (PP_MEMFAIL);
	}

	(void) DWLmemset(pPpCont, 0, sizeof(PPContainer));

	pPpCont->dwl = dwl;

	PPInitDataStructures(pPpCont);

	PPInitHW(pPpCont);

	if (PPSelectOutputSize(pPpCont) != PP_OK) {
		PP_API_TRC("PPInit# ERROR: Ilegal output size");
		DWLfree(pPpCont);
		(void) DWLRelease(dwl);
		return (PP_DWL_ERROR);
	}

	pPpCont->decInst = NULL;
	pPpCont->pipeline = 0;
	pPpCont->hwId = product;    /* save product id */
	PPSetStatus(pPpCont, PP_STATUS_IDLE);

#if ( PP_X170_FAST_VERTICAL_DOWNSCALE_DISABLE != 0 )
	pPpCont->fastVerticalDownscaleDisable = 1;
#endif

#if ( PP_X170_FAST_HORIZONTAL_DOWNSCALE_DISABLE != 0 )
	pPpCont->fastHorizontalDownscaleDisable = 1;
#endif

	*pPostPInst = pPpCont;
	//pthread_mutex_init(&pPpCont->pp_buf_mutex, NULL);
	//pthread_cond_init(&pPpCont->pp_buf_release_cv, NULL);
	return (PP_OK);
}

/*------------------------------------------------------------------------------
    Function name   : PPGetConfig
    Description     :
    Return type     : PPResult
    Argument        : PPInst postPInst
    Argument        : PPConfig * pPpConf
------------------------------------------------------------------------------*/
PPResult PPGetConfig(PPInst postPInst, PPConfig *pPpConf)
{
	PPContainer *ppC;

	PP_API_TRC("PPGetConfig #");

	if (postPInst == NULL || pPpConf == NULL) {
		return (PP_PARAM_ERROR);
	}

	ppC = (PPContainer *) postPInst;

	(void) DWLmemcpy(pPpConf, &ppC->ppCfg, sizeof(PPConfig));

	pPpConf->ppMaxSupportedOutRes.maxOutWidth = ppC->maxOutWidth;
	pPpConf->ppMaxSupportedOutRes.maxOutHeight = ppC->maxOutHeight;

	return PP_OK;
}

/*------------------------------------------------------------------------------
    Function name   : PPSetConfig
    Description     :
    Return type     : PPResult
    Argument        : PPInst postPInst
    Argument        : PPConfig * pPpConf
------------------------------------------------------------------------------*/
PPResult PPSetConfig(PPInst postPInst, PPConfig *pPpConf)
{

	PPContainer *ppC;

	PPOutImage *ppOutImg;
	PPInImage *ppInImg;
	PPOutRgb *ppOutRgb;
	PPRgbTransform *rgbT;

	i32 tmp;

	PP_API_TRC("PPSetConfig #\n");
	if (postPInst == NULL || pPpConf == NULL) {
		return (PP_PARAM_ERROR);
	}

	ppC = (PPContainer *) postPInst;

	ppOutImg = &ppC->ppCfg.ppOutImg;
	ppInImg = &ppC->ppCfg.ppInImg;
	ppOutRgb = &ppC->ppCfg.ppOutRgb;
	rgbT = &ppOutRgb->rgbTransformCoeffs;

	ppC->multiBuffer = pPpConf->ppOutImg.multiBuffer;

	if (ppC->multiBuffer) {
		/* we set these to make PPCheckConfig() happy */
		pPpConf->ppOutImg.bufferBusAddr =
			ppC->combinedModeBuffers.ppOutputBuffers[0].bufferBusAddr;
		pPpConf->ppOutImg.bufferChromaBusAddr =
			ppC->combinedModeBuffers.ppOutputBuffers[0].bufferChromaBusAddr;
	} else {
		/* If multibuffer mode is not used, then the amount of
		 * buffers is not initialized. */
		if (ppC->combinedModeBuffers.nbrOfBuffers == 0) {
			ppC->combinedModeBuffers.nbrOfBuffers = 1;
		}
	}

	tmp =
		PPCheckConfig(ppC, pPpConf, (ppC->decInst != NULL ? 1 : 0),
					  ppC->decType);

	if (tmp != (i32) PP_OK) {
		return (PPResult) tmp;
	}

	/* store previous cfg in order to notice any changes that require
	 * rerun of buffered pictures */
	(void) DWLmemcpy(&ppC->prevCfg, &ppC->ppCfg, sizeof(PPConfig));

	(void) DWLmemcpy(&ppC->ppCfg, pPpConf, sizeof(PPConfig));

	switch (ppOutRgb->rgbTransform) {
	case PP_YCBCR2RGB_TRANSFORM_CUSTOM:
		/* coeffs are coming from user */
		break;
	case PP_YCBCR2RGB_TRANSFORM_BT_601:
		/* Bt.601 */
		if (ppInImg->videoRange == 0) {
			rgbT->a = 298;
			rgbT->b = 409;
			rgbT->c = 208;
			rgbT->d = 100;
			rgbT->e = 516;
		} else {
			rgbT->a = 256;
			rgbT->b = 350;
			rgbT->c = 179;
			rgbT->d = 86;
			rgbT->e = 443;
		}
		break;
	case PP_YCBCR2RGB_TRANSFORM_BT_709:
		/* Bt.709 */
		if (ppInImg->videoRange == 0) {
			rgbT->a = 298;
			rgbT->b = 459;
			rgbT->c = 137;
			rgbT->d = 55;
			rgbT->e = 544;
		} else {
			rgbT->a = 256;
			rgbT->b = 403;
			rgbT->c = 120;
			rgbT->d = 48;
			rgbT->e = 475;
		}
		break;
	default:
		ASSERT(0);
	}

	/* initialize alpha blend source endianness to input picture endianness.
	 * Cannot be done in Init() because output format is configured here and
	 * PP_X170_IGNORE_ABLEND_ENDIANNESS flag affects only RGB32 */
	SetPpRegister(ppC->ppRegs, HWIF_PP_IN_A1_ENDIAN,
				  PP_X170_INPUT_PICTURE_ENDIAN);

	switch (ppOutImg->pixFormat & 0xFF0000) {
	case PP_PIXEL_FORMAT_RGB_MASK:
		ppC->outFormat = PP_ASIC_OUT_FORMAT_RGB;
		if (ppOutImg->pixFormat & PP_PIXEL_FORMAT_RGB32_MASK) {
			ppC->rgbDepth = 32;
#if ( PP_X170_IGNORE_ABLEND_ENDIANNESS != 0 )
			/* if input endianness ignored for alpha blend sources -> set big
			 * endian */
			SetPpRegister(ppC->ppRegs, HWIF_PP_IN_A1_ENDIAN,
						  PP_X170_PICTURE_BIG_ENDIAN);
#endif
		} else {
			ppC->rgbDepth = 16;
		}
		break;
	case PP_PIXEL_FORMAT_YUV420_MASK:
		ppC->outFormat = PP_ASIC_OUT_FORMAT_420;
		break;
	case PP_PIXEL_FORMAT_YUV422_MASK:
		ppC->outFormat = PP_ASIC_OUT_FORMAT_422;
		ppC->outStartCh =
			(ppOutImg->pixFormat == PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED ||
			 ppOutImg->pixFormat == PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED ||
			 ppOutImg->pixFormat == PP_PIX_FMT_CBYCRY_4_2_2_TILED_4X4 ||
			 ppOutImg->pixFormat == PP_PIX_FMT_CRYCBY_4_2_2_TILED_4X4);
		ppC->outCrFirst =
			(ppOutImg->pixFormat == PP_PIX_FMT_YCRYCB_4_2_2_INTERLEAVED ||
			 ppOutImg->pixFormat == PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED ||
			 ppOutImg->pixFormat == PP_PIX_FMT_YCRYCB_4_2_2_TILED_4X4 ||
			 ppOutImg->pixFormat == PP_PIX_FMT_CRYCBY_4_2_2_TILED_4X4);
		ppC->outTiled4x4 =
			(ppOutImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_2_TILED_4X4 ||
			 ppOutImg->pixFormat == PP_PIX_FMT_YCRYCB_4_2_2_TILED_4X4 ||
			 ppOutImg->pixFormat == PP_PIX_FMT_CBYCRY_4_2_2_TILED_4X4 ||
			 ppOutImg->pixFormat == PP_PIX_FMT_CRYCBY_4_2_2_TILED_4X4);
		break;
	default:
		ASSERT(0);
	}

	switch (ppInImg->pixFormat) {
	case PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED:
		ppC->inFormat = PP_ASIC_IN_FORMAT_422;
		ppC->inStartCh = 0;
		ppC->inCrFirst = 0;
		break;
	case PP_PIX_FMT_YCRYCB_4_2_2_INTERLEAVED:
		ppC->inFormat = PP_ASIC_IN_FORMAT_422;
		ppC->inStartCh = 0;
		ppC->inCrFirst = 1;
		break;
	case PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED:
		ppC->inFormat = PP_ASIC_IN_FORMAT_422;
		ppC->inStartCh = 1;
		ppC->inCrFirst = 0;
		break;
	case PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED:
		ppC->inFormat = PP_ASIC_IN_FORMAT_422;
		ppC->inStartCh = 1;
		ppC->inCrFirst = 1;
		break;
	case PP_PIX_FMT_YCBCR_4_2_0_PLANAR:
		ppC->inFormat = PP_ASIC_IN_FORMAT_420_PLANAR;
		break;
	case PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR:
		ppC->inFormat = PP_ASIC_IN_FORMAT_420_SEMIPLANAR;
		break;
	case PP_PIX_FMT_YCBCR_4_0_0:
		ppC->inFormat = PP_ASIC_IN_FORMAT_400;
		break;
	case PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR:
		ppC->inFormat = PP_ASIC_IN_FORMAT_422_SEMIPLANAR;
		break;
	case PP_PIX_FMT_YCBCR_4_2_0_TILED:
		//ppC->inFormat = PP_ASIC_IN_FORMAT_420_TILED;
		ppC->inFormat = PP_ASIC_IN_FORMAT_420_SEMIPLANAR;
		SetPpRegister(ppC->ppRegs, HWIF_PP_TILED_MODE, 1);
		break;
	case PP_PIX_FMT_YCBCR_4_4_0:
		ppC->inFormat = PP_ASIC_IN_FORMAT_440_SEMIPLANAR;
		break;
	case PP_PIX_FMT_YCBCR_4_1_1_SEMIPLANAR:
		ppC->inFormat = PP_ASIC_IN_FORMAT_411_SEMIPLANAR;
		break;
	case PP_PIX_FMT_YCBCR_4_4_4_SEMIPLANAR:
		ppC->inFormat = PP_ASIC_IN_FORMAT_444_SEMIPLANAR;
		break;
	default:
		ASSERT(0);
	}

	/* if config has changed in a way that messes up the multi-buffering,
	 * mark previously decoded pics dirty */

	if (ppC->multiBuffer) {
		if (PPCheckSetupChanges(&ppC->prevCfg, &ppC->ppCfg)) {
			ppC->currentSetupID++;
		}

	}

	PPSetupHW(ppC);

	return (PP_OK);
}

/*------------------------------------------------------------------------------
    Function name   : PPRelease
    Description     :
    Return type     : void
    Argument        : PPInst postPInst
------------------------------------------------------------------------------*/
void PPRelease(PPInst postPInst)
{
	PPContainer *ppC;
	const void *dwl;

	if (postPInst == NULL) {
		return;
	}

	ppC = (PPContainer *) postPInst;

	if (ppC->decInst != NULL) {
		(void) PPDecCombinedModeDisable(ppC, ppC->decInst);
	}

	//pthread_mutex_destroy(&ppC->pp_buf_mutex);
	//pthread_cond_destroy(&ppC->pp_buf_release_cv);
	dwl = ppC->dwl;
	DWLfree(ppC);
	(void) DWLRelease((void *) dwl);
}

/*------------------------------------------------------------------------------
    Function name   : PPGetResult
    Description     :
    Return type     : PPResult
    Argument        : PPInst postPInst
------------------------------------------------------------------------------*/
PPResult PPGetResult(PPInst postPInst)
{
	PPContainer *ppC;
	PPResult ret = PP_OK;

	PP_API_TRC("PPGetResult #");
	if (postPInst == NULL) {
		return (PP_PARAM_ERROR);
	}

	ppC = (PPContainer *) postPInst;

	if (PPGetStatus(ppC) != PP_STATUS_IDLE) {
		return PP_BUSY;
	}

	if (PP_IS_JOINED(ppC)) {
		ret = ppC->PPCombinedRet;
	} else {
		if (PPRun(ppC) != PP_OK) {
			return PP_BUSY;
		}

		ret = WaitForPp(ppC);
	}

	PP_API_TRC("PPGetResult # exit");
	return ret;
}

/*------------------------------------------------------------------------------
    Function name   : PPDecWaitResult
    Description     : Wait for PP, not started here i.e PPRun not called
    Return type     : PPResult
    Argument        : PPInst postPInst
------------------------------------------------------------------------------*/
PPResult PPDecWaitResult(PPInst postPInst)
{
	PPContainer *ppC;
	PPResult ret = PP_OK;

	PP_API_TRC("PPGetResult #");

	if (postPInst == NULL) {
		PP_API_TRC("PPGetResult paramerr");
		return (PP_PARAM_ERROR);
	}

	ppC = (PPContainer *) postPInst;

	ASSERT(ppC->pipeline == 0);

	if (ppC->decInst == NULL) {
		PP_API_TRC("PPGetResult paramerr");
		return (PP_PARAM_ERROR);
	}

	if (PPGetStatus(ppC) != PP_STATUS_RUNNING) {

		PP_API_TRC("PPGetResult pp_busy");
		return PP_BUSY;
	}

	ret = WaitForPp(ppC);
	PP_API_TRC("PPGetResult # exit");
	return ret;
}

/*------------------------------------------------------------------------------
    Function name   : PPGetAPIVersion
    Description     :
    Return type     : PPApiVersion
------------------------------------------------------------------------------*/
PPApiVersion PPGetAPIVersion()
{
	PPApiVersion ver;

	ver.major = PP_MAJOR_VERSION;
	ver.minor = PP_MINOR_VERSION;

	PP_API_TRC("PPGetAPIVersion");

	return ver;
}

/*------------------------------------------------------------------------------
    Function name   : PPGetBuild
    Description     :
    Return type     : PPBuild
    Argument        : void
------------------------------------------------------------------------------*/
PPBuild PPGetBuild(void)
{
	PPBuild buildInfo;

	(void)DWLmemset(&buildInfo, 0, sizeof(buildInfo));

	buildInfo.swBuild = HANTRO_DEC_SW_BUILD;
	buildInfo.hwBuild = DWLReadAsicID();

	DWLReadAsicConfig(buildInfo.hwConfig);

	PP_API_TRC("PPGetBuild#");

	return buildInfo;
}

/*------------------------------------------------------------------------------
    Function name   : PPDecStartPp
    Description     : set up and run pp based on information from the decoder
    Return type     : void
    Argument        : pp instance and control information struct
------------------------------------------------------------------------------*/

#ifdef PP_DEC_PIPELINE_SUPPORT

static void PPDecStartPp(const void *postPInst, const DecPpInterface *decpp)
{
	PPContainer *ppC;
	PPInImage *ppInImg;
	PPOutImage *ppOutImg;
	PPOutDeinterlace *ppOutDeint;
	u32 tmp = 0;
	u32 pixFormatOk;
	u32 crop8_d_enable = 0;

	PP_API_TRC("PPDecStartPp #");
	ppC = (PPContainer *) postPInst;

	ASSERT(ppC != NULL);
	ASSERT(decpp != NULL);

	if (ppC == NULL) {
		return;
	}

	if (decpp == NULL) {
		ppC->PPCombinedRet = PP_PARAM_ERROR;
		return;
	}

	ppC->PPCombinedRet = PP_OK;

	ppOutDeint = &ppC->ppCfg.ppOutDeinterlace;

	ppInImg = &ppC->ppCfg.ppInImg;
	ppOutImg = &ppC->ppCfg.ppOutImg;

	if (decpp->tiledInputMode) {
		ppC->fastVerticalDownscaleDisable = 1;
	}

	/* PP does not currently support arbitrary luma/chroma strides so bail
	 * out if they are used. */
	if (decpp->lumaStride && decpp->lumaStride != ppInImg->width) {
		ppC->PPCombinedRet = PP_DEC_RUNTIME_ERROR;
		return;
	}

	if (decpp->chromaStride && decpp->chromaStride != ppInImg->width) {
		ppC->PPCombinedRet = PP_DEC_RUNTIME_ERROR;
		return;
	}

	/* Set VC-1 specific parameters */
	if (ppC->decType == PP_PIPELINED_DEC_TYPE_VC1) {

		ppC->inWidth = decpp->croppedW;
		ppC->inHeight = decpp->croppedH;

		PPSetupScaling(ppC, ppOutImg);

		/* HW must discard 8 pixels from input picture.
		 * Notice that decpp->inwidth is always multiple of 16 and
		 * decpp->croppedW is always multiple of 8. Same for height. */
		if (decpp->inwidth != decpp->croppedW) {
			SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_R_E, 1);
		} else {
			SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_R_E, 0);
		}

		if (decpp->inheight != decpp->croppedH) {
			SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_D_E, 1);
		}

		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_WIDTH, decpp->inwidth >> 4);
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_HEIGHT, decpp->inheight >> 4);

		SetPpRegister(ppC->ppRegs, HWIF_PP_VC1_ADV_E, decpp->vc1AdvEnable);

		if (decpp->rangeRed) {
			SetPpRegister(ppC->ppRegs, HWIF_RANGEMAP_Y_E, 1);
			SetPpRegister(ppC->ppRegs, HWIF_RANGEMAP_COEF_Y, 7 + 9);
			SetPpRegister(ppC->ppRegs, HWIF_RANGEMAP_C_E, 1);
			SetPpRegister(ppC->ppRegs, HWIF_RANGEMAP_COEF_C, 7 + 9);
		} else {
			SetPpRegister(ppC->ppRegs,
						  HWIF_RANGEMAP_Y_E, decpp->rangeMapYEnable);
			SetPpRegister(ppC->ppRegs,
						  HWIF_RANGEMAP_COEF_Y, decpp->rangeMapYCoeff + 9);
			SetPpRegister(ppC->ppRegs,
						  HWIF_RANGEMAP_C_E, decpp->rangeMapCEnable);
			SetPpRegister(ppC->ppRegs,
						  HWIF_RANGEMAP_COEF_C, decpp->rangeMapCCoeff + 9);
		}
	} else if (ppC->decType == PP_PIPELINED_DEC_TYPE_MPEG4 || ppC->decType == PP_PIPELINED_DEC_TYPE_H264) {
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_HEIGHT, decpp->inheight >> 4);
		/* crop extra block if height is odd and fields */
		if (decpp->croppedH != decpp->inheight && decpp->inheight - decpp->croppedH <= 8) {
			SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_D_E, 1);
			crop8_d_enable = 1;
		}
	}

	/* update output buffer when multibuffering used */
	if ((ppC->multiBuffer != 0) &&
		(ppC->decType == PP_PIPELINED_DEC_TYPE_MPEG4 ||
		 ppC->decType == PP_PIPELINED_DEC_TYPE_MPEG2 ||
		 ppC->decType == PP_PIPELINED_DEC_TYPE_H264 ||
		 ppC->decType == PP_PIPELINED_DEC_TYPE_VC1 ||
		 ppC->decType == PP_PIPELINED_DEC_TYPE_RV ||
		 ppC->decType == PP_PIPELINED_DEC_TYPE_AVS ||
		 ppC->decType == PP_PIPELINED_DEC_TYPE_VP8 ||
		 ppC->decType == PP_PIPELINED_DEC_TYPE_WEBP)) {
		if (decpp->bufferIndex >= ppC->combinedModeBuffers.nbrOfBuffers) {
			PP_API_TRC
			("PPDecStartPp # exit runtime error (out-of-range buffer index)");
			ppC->PPCombinedRet = PP_PARAM_ERROR;
			return;
		}

		PPDecSetOutBuffer(ppC, decpp);
	}

	/* If tiled-mode supported on PP, forward status of current picture */
	if (decpp->tiledInputMode) {
		if (!ppC->tiledModeSupport) {
			ppC->PPCombinedRet = PP_DEC_RUNTIME_ERROR;
			return;
		}
		SetPpRegister(ppC->ppRegs, HWIF_PP_TILED_MODE, decpp->tiledInputMode);
	} else {
		SetPpRegister(ppC->ppRegs, HWIF_PP_TILED_MODE, 0);
	}

	if (decpp->progressiveSequence &&
		ppOutDeint->enable) {
		ppC->PPCombinedRet = PP_DEC_RUNTIME_ERROR;
		return;
	}

	/* check that input size is same as set in PPSetConfig.
	 * ppInImg->width is rounded up to next multiple of 8 as well as croppedW */
	if (decpp->croppedW != ppInImg->width) {
		PP_API_TRC("PPDecStartPp # exit runtime error");
		ppC->PPCombinedRet = PP_DEC_RUNTIME_ERROR;
		return;
	}

	/* Double-check picture dimensions here in case pipeline turned
	 * off at some point */
	{
		/* Max size for pipelined pic, 511MB for JPEG etc, 1024MB for WEBP */
		u32 maxMbs =
			((ppC->decType == PP_PIPELINED_DEC_TYPE_WEBP) ||
			 ((ppC->decType == PP_PIPELINED_DEC_TYPE_JPEG) &&
			  ppC->jpeg16kSupport)) ?
			1024U : 511U;
		if ((ppInImg->width < PP_IN_MIN_WIDTH(decpp->usePipeline)) ||
			(ppInImg->height < PP_IN_MIN_HEIGHT(decpp->usePipeline)) ||
			(ppInImg->width > PP_IN_MAX_WIDTH_EXT(decpp->usePipeline, maxMbs)) ||
			(ppInImg->height > PP_IN_MAX_HEIGHT_EXT(decpp->usePipeline, maxMbs))) {
			ppC->PPCombinedRet = PP_DEC_RUNTIME_ERROR;
			return;
		}
	}

	if (decpp->croppedH != ppInImg->height && !crop8_d_enable) {
		PP_API_TRC("PPDecStartPp # exit runtime error");
		ppC->PPCombinedRet = PP_DEC_RUNTIME_ERROR;
		return;
	}

	/* Set PP input image dimensions. Cropping dimensions are used
	 * if Zooming is enabled in pp configuration */
	if (ppC->ppCfg.ppInCrop.enable) {
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_W_EXT,
					  ((((ppC->ppCfg.ppInCrop.width + 15) >> 4) & 0xE00) >> 9));
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_WIDTH,
					  (((ppC->ppCfg.ppInCrop.width + 15) >> 4) & 0x1FF));
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_H_EXT,
					  ((((ppC->ppCfg.ppInCrop.height +
						  15) >> 4) & 0x700) >> 8));
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_HEIGHT,
					  (((ppC->ppCfg.ppInCrop.height + 15) >> 4) & 0x0FF));
	}

	if (decpp->usePipeline) {
		PP_API_TRC("pipelined #\n");
		ppC->pipeline = 1;
		SetPpRegister(ppC->ppRegs, HWIF_PP_PIPELINE_E, 1);
	} else {
		/* check for valid picStruct for deinterlacing */
		if (ppOutDeint->enable &&
			((decpp->picStruct == DECPP_PIC_TOP_AND_BOT_FIELD_FRAME) ||
			 (decpp->picStruct == DECPP_PIC_TOP_AND_BOT_FIELD)) &&
			!ppC->fastVerticalDownscale) {
			PPDecSetUpDeinterlace(ppC, decpp);
		} else {
			/* disable deinterlacing for any other picStruct */
			SetPpRegister(ppC->ppRegs, HWIF_DEINT_E, 0);
			PPDEBUG_PRINT(("Deinterlacing DISABLED because not valid dec input!\n"));

			SetPpRegister(ppC->ppRegs, HWIF_PP_IN_STRUCT, decpp->picStruct);

#if 0
			if (decpp->picStruct == DECPP_PIC_TOP_FIELD_FRAME ||
				decpp->picStruct == DECPP_PIC_FRAME_OR_TOP_FIELD) {

				SetPpRegister(ppC->ppRegs, HWIF_PP_IN_LU_BASE,
							  decpp->inputBusLuma);
				SetPpRegister(ppC->ppRegs, HWIF_PP_IN_CB_BASE,
							  decpp->inputBusChroma);

			} else {
				SetPpRegister(ppC->ppRegs, HWIF_PP_BOT_YIN_BASE,
							  decpp->bottomBusLuma);
				SetPpRegister(ppC->ppRegs, HWIF_PP_BOT_CIN_BASE,
							  decpp->bottomBusChroma);
			}
#endif
			/* set all base addresses */
			/* based on HWIF_PP_IN_STRUCT, HW will use the relevant ones only */
			SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_IN_LU_BASE, decpp->inputBusLuma);

			if (ppInImg->pixFormat == PP_PIX_FMT_YCBCR_4_0_0) {
				/* this is workaround for standalone 4:0:0 in HW */
				SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_IN_CB_BASE,
								decpp->inputBusLuma);
				SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_BOT_YIN_BASE,
								decpp->bottomBusLuma);
				SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_BOT_CIN_BASE,
								decpp->bottomBusLuma);
			} else {
				SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_IN_CB_BASE,
								decpp->inputBusChroma);
				SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_BOT_YIN_BASE,
								decpp->bottomBusLuma);
				SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_BOT_CIN_BASE,
								decpp->bottomBusChroma);
			}
		}
		PP_API_TRC("non pipelined #\n");
		ppC->pipeline = 0;
		SetPpRegister(ppC->ppRegs, HWIF_PP_PIPELINE_E, 0);

		tmp = decpp->littleEndian ?
			  PP_X170_PICTURE_LITTLE_ENDIAN : PP_X170_PICTURE_BIG_ENDIAN;

		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_ENDIAN, tmp);

		tmp = decpp->wordSwap ? 1 : 0;
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_SWAP32_E, tmp);
	}

	/* Only 4:2:0 and 4:0:0 inputs supported */
	pixFormatOk =
		(ppInImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_0_PLANAR ||
		 ppInImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR ||
		 ppInImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_0_TILED ||
		 ppInImg->pixFormat == PP_PIX_FMT_YCBCR_4_0_0);

	/* Check for faster downscale possibility */
	if (ppC->fastVerticalDownscale) {
		u32 inHeight;
		u32 rotation90;

		/* Determine input height */
		if (ppC->ppCfg.ppInCrop.enable) {
			inHeight = ppC->ppCfg.ppInCrop.height;
		} else {
			inHeight = ppInImg->height;
		}

		/* Rotation */
		rotation90 = (ppC->ppCfg.ppInRotation.rotation == PP_ROTATION_LEFT_90 ||
					  ppC->ppCfg.ppInRotation.rotation == PP_ROTATION_RIGHT_90);

		/* Vertical downscaling shortcut available if:
		 * - no pipeline
		 * - progressive input data
		 * - pixel format either 4:2:0 or 4:0:0
		 * If rotation is used then shortcut affects horizontal scaling, at
		 * which point we need to cross-check with fast horizontal scaling
		 * mode as well.
		 */
		if (ppC->pipeline == 0 &&
			pixFormatOk &&
			!decpp->tiledInputMode &&
			(decpp->picStruct == PP_PIC_FRAME_OR_TOP_FIELD ||
			 (ppOutDeint->enable &&
			  decpp->picStruct == PP_PIC_TOP_AND_BOT_FIELD_FRAME))) {

			/* possible */

			/* Set new image height and cropping parameters */
			SetPpRegister(ppC->ppRegs, HWIF_PP_IN_STRUCT, PP_PIC_TOP_FIELD_FRAME);
			SetPpRegister(ppC->ppRegs, HWIF_PP_IN_H_EXT,
						  ((((inHeight + 31) / 32) & 0x700) >> 8));
			SetPpRegister(ppC->ppRegs, HWIF_PP_IN_HEIGHT,
						  (((inHeight + 31) / 32) & 0x0FF));
			if (inHeight & 0x1F) {
				SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_D_E, 1);
			} else {
				SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_D_E, 0);
			}

			if (ppC->ppCfg.ppInCrop.enable) {
				SetPpRegister(ppC->ppRegs, HWIF_CROP_STARTY_EXT,
							  (((ppC->ppCfg.ppInCrop.originY / 32) & 0x700) >> 8));
				SetPpRegister(ppC->ppRegs, HWIF_CROP_STARTY,
							  ((ppC->ppCfg.ppInCrop.originY / 32) & 0x0FF));
			}

			/* Disable deinterlacing */
			SetPpRegister(ppC->ppRegs, HWIF_DEINT_E, 0);

			/* Set new scaling coefficient */
			if (rotation90) {
				/* Set fast scale mode */
				SetPpRegister(ppC->ppRegs, HWIF_HOR_SCALE_MODE, ppC->fastScaleMode);
				if (ppC->fastHorizontalDownscale) {
					SetPpRegister(ppC->ppRegs, HWIF_WSCALE_INVRA, ppC->cHfast4x);
				} else {
					SetPpRegister(ppC->ppRegs, HWIF_WSCALE_INVRA, ppC->cHfast);
				}
			} else {
				/* Set fast scale mode */
				SetPpRegister(ppC->ppRegs, HWIF_VER_SCALE_MODE, ppC->fastScaleMode);

				SetPpRegister(ppC->ppRegs, HWIF_HSCALE_INVRA, ppC->cVfast);
			}
		} else {
			/* not possible */

			SetPpRegister(ppC->ppRegs, HWIF_PP_IN_H_EXT,
						  ((((inHeight + 15) / 16) & 0x700) >> 8));
			SetPpRegister(ppC->ppRegs, HWIF_PP_IN_HEIGHT,
						  (((inHeight + 15) / 16) & 0x0FF));
			if (inHeight & 0x0F) {
				SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_D_E, 1);
			} else {
				SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_D_E, 0);
			}

			if (ppC->ppCfg.ppInCrop.enable) {
				SetPpRegister(ppC->ppRegs, HWIF_CROP_STARTY_EXT,
							  (((ppC->ppCfg.ppInCrop.originY / 16) & 0x700) >> 8));
				SetPpRegister(ppC->ppRegs, HWIF_CROP_STARTY,
							  ((ppC->ppCfg.ppInCrop.originY / 16) & 0x0FF));
			}

			if (rotation90) {
				SetPpRegister(ppC->ppRegs, HWIF_HOR_SCALE_MODE, 2);
				if (ppC->fastHorizontalDownscale) {
					SetPpRegister(ppC->ppRegs, HWIF_WSCALE_INVRA, ppC->cHfast);
				} else {
					SetPpRegister(ppC->ppRegs, HWIF_WSCALE_INVRA, ppC->cHnorm);
				}
			} else {
				SetPpRegister(ppC->ppRegs, HWIF_VER_SCALE_MODE, 2);
				SetPpRegister(ppC->ppRegs, HWIF_HSCALE_INVRA, ppC->cVnorm);
			}
		}
	}

	/* Disallow fast horizontal scale if 4:1:1 sampling is used */
	if (ppC->fastHorizontalDownscale && !pixFormatOk) {
		SetPpRegister(ppC->ppRegs, HWIF_PP_FAST_SCALE_E, 0);
		SetPpRegister(ppC->ppRegs, HWIF_WSCALE_INVRA, ppC->cHnorm);
	}

	if (PPRun(ppC) != PP_OK) {
		PP_API_TRC("PPDecStartPp # exit PPRun failed");
		ppC->PPCombinedRet = PP_BUSY;
		return;
	}

	ppC->PPCombinedRet = PP_OK;
	PP_API_TRC("PPDecStartPp # exit");
	return;

}

/*------------------------------------------------------------------------------
    Function name   : PPDecConfigQueryFromDec
                        Used by decoder to ask about pp setup
    Description     : if pipeline proposed, evaluate reasons who not
    Return type     : void
    Argument        : const void *postPInst
------------------------------------------------------------------------------*/
static void PPDecConfigQueryFromDec(const void *postPInst,
									DecPpQuery *decPpQuery)
{
	PPContainer *ppC;

	ASSERT(postPInst != NULL);
	if (postPInst == NULL) {
		return;
	}
	ASSERT(decPpQuery != NULL);
	if (decPpQuery == NULL) {
		return;
	}

	ppC = (PPContainer *) postPInst;

	decPpQuery->pipelineAccepted = 1;
	decPpQuery->ppConfigChanged = 0;

	decPpQuery->deinterlace = ppC->ppCfg.ppOutDeinterlace.enable ? 1 : 0;

	decPpQuery->multiBuffer = ppC->multiBuffer;

	if (decPpQuery->deinterlace) {
		decPpQuery->pipelineAccepted = 0;
	}

	/* Fast vertical downscale shortcut disables pipeline automatically; however
	 * at some cases SW might still be thinking that it can be used even though
	 * the input pixel format is invalid. */
	if (ppC->fastVerticalDownscale && !ppC->fastVerticalDownscaleDisable &&
		(ppC->ppCfg.ppInImg.pixFormat == PP_PIX_FMT_YCBCR_4_2_0_PLANAR ||
		 ppC->ppCfg.ppInImg.pixFormat == PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR ||
		 ppC->ppCfg.ppInImg.pixFormat == PP_PIX_FMT_YCBCR_4_2_0_TILED ||
		 ppC->ppCfg.ppInImg.pixFormat == PP_PIX_FMT_YCBCR_4_0_0)) {
		decPpQuery->pipelineAccepted = 0;
	}

	if (ppC->ppCfg.ppInRotation.rotation != PP_ROTATION_NONE) {
		decPpQuery->pipelineAccepted = 0;

	}

	if (ppC->ppCfg.ppInCrop.enable != 0 &&
		ppC->decType != PP_PIPELINED_DEC_TYPE_JPEG) {
		u32 tmp = decPpQuery->pipelineAccepted;
		decPpQuery->pipelineAccepted = 0;
		/* pipelined cropping allowed if webp width > max video width
		 * or height >= 8176 px.
		 * (this implies filter turned off which in turn enables
		 *  pipelined cropping). */
		if ((ppC->decType == PP_PIPELINED_DEC_TYPE_WEBP) ||
			((ppC->decType == PP_PIPELINED_DEC_TYPE_JPEG) && ppC->jpeg16kSupport)) {
			DWLHwConfig_t hwCfg;
			DWLReadAsicConfig(&hwCfg);
			if (ppC->ppCfg.ppInImg.width > hwCfg.maxDecPicWidth ||
				ppC->ppCfg.ppInImg.height >= 8176) { /* rollback */
				decPpQuery->pipelineAccepted = tmp;
			}
		}
	}

	if (ppC->ppCfg.ppInRotation.rotation == PP_ROTATION_NONE &&
		ppC->prevCfg.ppInRotation.rotation != PP_ROTATION_NONE) {
		decPpQuery->ppConfigChanged = 1;
	}

	/* Some formats' pipelined block size limits max vertical upscaling ratio
	 * to 2.5x */
	if (ppC->decType == PP_PIPELINED_DEC_TYPE_RV ||
		ppC->decType == PP_PIPELINED_DEC_TYPE_VP6 ||
		ppC->decType == PP_PIPELINED_DEC_TYPE_VP8) {
		if ((ppC->ppCfg.ppOutImg.height > ppC->ppCfg.ppInImg.height) &&
			(2 * ppC->ppCfg.ppOutImg.height >
			 (5 * ppC->ppCfg.ppInImg.height - 2))) {
			decPpQuery->pipelineAccepted = 0;
		}
	}

	return;
}

/*------------------------------------------------------------------------------
    Function name   : PPDecEndCallback
    Description     : Decoder/pp cooperation ending
    Return type     : void
    Argument        : const void *postPInst
------------------------------------------------------------------------------*/
void PPDecEndCallback(const void *postPInst)
{
	PPContainer *ppC;
	PPResult ppRet;
	if (postPInst == NULL) {
		PP_API_TRC("PPInit# ERROR: PPInst == NULL");
		return;
	}
	PP_API_TRC("PPDecEndCallback #");
	ppC = (PPContainer *) postPInst;

	if (ppC->PPCombinedRet == PP_OK) {
		if (ppC->pipeline) {
			PPRefreshRegs(ppC);

			/* turn pipeline OFF so it doesn't interfere with other instances */
			ppC->pipeline = 0;
			SetPpRegister(ppC->ppRegs, HWIF_PP_PIPELINE_E, 0);

			/* make sure ASIC is OFF */
			SetPpRegister(ppC->ppRegs, HWIF_PP_E, 0);
			SetPpRegister(ppC->ppRegs, HWIF_PP_IRQ, 0);
			SetPpRegister(ppC->ppRegs, HWIF_PP_IRQ_STAT, 0);

			DWLDisableHW(ppC->dwl, ppC->coreID, PP_X170_REG_START,
						 ppC->ppRegs[0]);

			/* H264/VP8 decoder has reserved the PP hardware for pipelined
			 * operation and it will release it too
			 */
			if (ppC->decType != PP_PIPELINED_DEC_TYPE_H264 &&
				ppC->decType != PP_PIPELINED_DEC_TYPE_VP8) {
				DWLReleaseHw(ppC->dwl, ppC->coreID);
			}

			PPSetStatus(ppC, PP_STATUS_IDLE);
		} else {
			ppRet = PPDecWaitResult(postPInst);
			ppC->PPCombinedRet = ppRet;
		}
	}
	PP_API_TRC("PPDecEndCallback # exit");
}

/*------------------------------------------------------------------------------
    Function name   : PPDecSetUpDeinterlace
    Description     : Decoder/pp cooperation, set up reg for deinterlace
    Return type     : void
    Argument        : void *postPInst, DecPpInterface
------------------------------------------------------------------------------*/
static void PPDecSetUpDeinterlace(PPContainer *ppC,
								  const DecPpInterface *decpp)
{

	if (ppC == NULL) {
		return;
	}
	ASSERT((decpp->picStruct == DECPP_PIC_TOP_AND_BOT_FIELD_FRAME) ||
		   (decpp->picStruct == DECPP_PIC_TOP_AND_BOT_FIELD));

	SetPpRegister(ppC->ppRegs, HWIF_PP_IN_STRUCT, decpp->picStruct);

	if (ppC->ppCfg.ppInImg.pixFormat == PP_PIX_FMT_YCBCR_4_0_0) {
		SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_IN_LU_BASE, decpp->inputBusLuma);
		SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_IN_CB_BASE, decpp->inputBusLuma);

		SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_BOT_YIN_BASE, decpp->bottomBusLuma);
		SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_BOT_CIN_BASE, decpp->bottomBusLuma);
	} else {
		SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_IN_LU_BASE, decpp->inputBusLuma);
		SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_IN_CB_BASE, decpp->inputBusChroma);

		SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_BOT_YIN_BASE, decpp->bottomBusLuma);
		SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_BOT_CIN_BASE,
						decpp->bottomBusChroma);
	}

}


#endif /* PP_DEC_PIPELINE_SUPPORT */

/*------------------------------------------------------------------------------
    Function name   : PPDecCombinedModeEnable
    Description     :
    Return type     : PPResult
    Argument        : PPInst postPInst
    Argument        : const void *pDecInst
    Argument        : u32 decType
------------------------------------------------------------------------------*/
PPResult PPDecCombinedModeEnable(PPInst postPInst, const void *pDecInst,
								 u32 decType)
{
	PPContainer *ppC;
	i32 ret = ~0;

	ppC = (PPContainer *) postPInst;
	PP_API_TRC("Pipeline enable #");
	PP_API_TRC("Pipeline enable #");

	if (ppC == NULL) {
		PP_API_TRC("PPDecCombinedModeEnable# ERROR: NULL pp instance");
		return PP_PARAM_ERROR;
	}

	if (pDecInst == NULL) {
		PP_API_TRC("PPDecCombinedModeEnable# ERROR: NULL decoder instance");
		return (PP_PARAM_ERROR);
	}

	if (decType == 0 || decType > 11) {
		PP_API_TRC
		("PPDecCombinedModeEnable# ERROR: decoder type not defined correctly");
		return (PP_PARAM_ERROR);
	}

	if (PPGetStatus(ppC) != PP_STATUS_IDLE) {
		PP_API_TRC("PPDecCombinedModeEnable# ERROR: pp status not idle");
		return PP_BUSY;
	}

	if (ppC->decInst != NULL) {
		PP_API_TRC
		("PPDecCombinedModeEnable# ERROR: PP already connected to a decoder");
		return PP_DEC_COMBINED_MODE_ERROR;
	}

	switch (decType) {

#ifdef PP_H264DEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_H264:
		PPDEBUG_PRINT(("Pipeline enable h264 #\n"));
		ret = h264RegisterPP(pDecInst, ppC, PPDecStartPp,
							 PPDecEndCallback, PPDecConfigQueryFromDec,
							 PPDecDisplayIndex, PPGetFreeMultiBufferId,
							 PPReturnMultiBufferId, PPFindDisplayId, PPRecoverMemInfo,
							 PPResetMultiBuffer, PPAddMultiBuffer,
							 PPGetNextOutput);
		break;
#endif

#ifdef PP_MPEG4DEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_MPEG4:
		PPDEBUG_PRINT(("Pipeline enable mpeg-4 #\n"));
		ret = mpeg4RegisterPP(pDecInst, ppC, PPDecStartPp,
							  PPDecEndCallback, PPDecConfigQueryFromDec,
							  PPDecDisplayIndex, PPDecBufferData);
		break;
#endif

#ifdef PP_JPEGDEC_PIPELINE_SUPPORT
		PPDEBUG_PRINT(("Pipeline enable JPEG #\n"));
	case PP_PIPELINED_DEC_TYPE_JPEG:
		ret = jpegRegisterPP(pDecInst, ppC, PPDecStartPp,
							 PPDecEndCallback, PPDecConfigQueryFromDec);

		break;
#endif

#ifdef PP_VC1DEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_VC1:
		PPDEBUG_PRINT(("Pipeline enable vc-1 #\n"));
		ret = vc1RegisterPP(pDecInst, ppC, PPDecStartPp,
							PPDecEndCallback, PPDecConfigQueryFromDec,
							PPDecDisplayIndex, PPDecBufferData);
		break;
#endif

#ifdef PP_MPEG2DEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_MPEG2:
		PPDEBUG_PRINT(("Pipeline enable mpeg-2 #\n"));
		ret = mpeg2RegisterPP(pDecInst, ppC, PPDecStartPp,
							  PPDecEndCallback, PPDecConfigQueryFromDec,
							  PPDecDisplayIndex, PPDecBufferData);
		break;
#endif

#ifdef PP_VP6DEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_VP6:
		PPDEBUG_PRINT(("Pipeline enable VP6 #\n"));
		ret = vp6RegisterPP(pDecInst, ppC, PPDecStartPp,
							PPDecEndCallback, PPDecConfigQueryFromDec);
		break;
#endif

#ifdef PP_VP8DEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_VP8:
	case PP_PIPELINED_DEC_TYPE_WEBP:
		PPDEBUG_PRINT(("Pipeline enable VP8 #\n"));
		ret = vp8RegisterPP(pDecInst, ppC, PPDecStartPp, PPDecEndCallback,
							PPDecConfigQueryFromDec, PPDecDisplayIndex, PPGetFreeMultiBufferId,
							PPReturnMultiBufferId, PPFindDisplayId, PPResetMultiBuffer,
							PPAddMultiBuffer, PPGetNextOutput, PPGetResult, PPDecBufferData);
		break;
#endif

#ifdef PP_RVDEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_RV:
		PPDEBUG_PRINT(("Pipeline enable real #\n"));
		ret = rvRegisterPP(pDecInst, ppC, PPDecStartPp,
						   PPDecEndCallback, PPDecConfigQueryFromDec,
						   PPDecDisplayIndex, PPDecBufferData);
		break;
#endif

#ifdef PP_AVSDEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_AVS:
		PPDEBUG_PRINT(("Pipeline enable AVS #\n"));
		ret = avsRegisterPP(pDecInst, ppC, PPDecStartPp,
							PPDecEndCallback, PPDecConfigQueryFromDec,
							PPDecDisplayIndex, PPDecBufferData);
		break;
#endif
	default:
		PP_API_TRC("PPDecCombinedModeEnable# unknown decoder type \n");
		return PP_PARAM_ERROR;
	}

	/*lint -e(527)   */
	if (ret != 0) {
		return PP_DEC_COMBINED_MODE_ERROR;
	}

	ppC->decType = decType;
	ppC->decInst = pDecInst;

	PP_API_TRC("PPDecCombinedModeEnable# OK \n");
	return PP_OK;
}

/*------------------------------------------------------------------------------
    Function name   : PPDecCombinedModeDisable
    Description     : decouple pp and dec
    Return type     : PPResult
    Argument        : PPInst postPInst
    Argument        : const void *pDecInst
------------------------------------------------------------------------------*/
PPResult PPDecCombinedModeDisable(PPInst postPInst, const void *pDecInst)
{
	PPContainer *ppC;

	ppC = (PPContainer *) postPInst;

	PP_API_TRC("PPDecCombinedModeDisable \n");
	ASSERT(ppC != NULL && pDecInst != NULL);

	if (ppC == NULL || pDecInst == NULL) {
		return PP_PARAM_ERROR;
	}

	if (ppC->decInst != pDecInst) {
		return PP_DEC_COMBINED_MODE_ERROR;
	}

	if (PPGetStatus(ppC) != PP_STATUS_IDLE) {
		return PP_BUSY;
	}

	switch (ppC->decType) {
#ifdef PP_H264DEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_H264:
		PPDEBUG_PRINT(("Pipeline disable h264 #\n"));
		(void) h264UnregisterPP(ppC->decInst, ppC);
		ppC->decInst = NULL;
		break;
#endif

#ifdef PP_MPEG4DEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_MPEG4:
		PPDEBUG_PRINT(("Pipeline disable mpeg4 #\n"));
		(void) mpeg4UnregisterPP(ppC->decInst, ppC);
		ppC->decInst = NULL;
		break;
#endif

#ifdef PP_JPEGDEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_JPEG:
		(void) jpegUnregisterPP(ppC->decInst, ppC);
		ppC->decInst = NULL;
		break;
#endif

#ifdef PP_VC1DEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_VC1:
		PPDEBUG_PRINT(("Pipeline disable vc1 #\n"));
		(void) vc1UnregisterPP(ppC->decInst, ppC);
		ppC->decInst = NULL;
		break;
#endif

#ifdef PP_MPEG2DEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_MPEG2:
		PPDEBUG_PRINT(("Pipeline disable mpeg2 #\n"));
		(void) mpeg2UnregisterPP(ppC->decInst, ppC);
		ppC->decInst = NULL;
		break;
#endif

#ifdef PP_RVDEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_RV:
		PPDEBUG_PRINT(("Pipeline disable real #\n"));
		(void) rvUnregisterPP(ppC->decInst, ppC);
		ppC->decInst = NULL;
		break;
#endif

#ifdef PP_VP6DEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_VP6:
		PPDEBUG_PRINT(("Pipeline disable VP6 #\n"));
		(void) vp6UnregisterPP(ppC->decInst, ppC);
		ppC->decInst = NULL;
		break;
#endif

#ifdef PP_VP8DEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_VP8:
	case PP_PIPELINED_DEC_TYPE_WEBP:
		PPDEBUG_PRINT(("Pipeline disable VP8 #\n"));
		(void) vp8UnregisterPP(ppC->decInst, ppC);
		ppC->decInst = NULL;
		break;
#endif

#ifdef PP_AVSDEC_PIPELINE_SUPPORT
	case PP_PIPELINED_DEC_TYPE_AVS:
		PPDEBUG_PRINT(("Pipeline disable avs #\n"));
		(void) avsUnregisterPP(ppC->decInst, ppC);
		ppC->decInst = NULL;
		break;
#endif

	default:
		ASSERT(0);  /* should never get here */
		break;
	}

	SetPpRegister(ppC->ppRegs, HWIF_PP_PIPELINE_E, 0);

	ppC->pipeline = 0;
	ppC->decInst = NULL;

	PP_API_TRC("PPDecCombinedModeDisable OK \n");
	return PP_OK;
}

PPResult PPDecSwapLastOutputBuffer(PPInst postPInst,
								   PPOutput *pOldBuffer,
								   PPOutput *pNewBuffer)
{
	PPContainer *ppC;
	PPOutput *lastPPOut;

	ppC = (PPContainer *) postPInst;

	PP_API_TRC("PPDecSwapLastOutputBuffer\n");

	if (ppC == NULL || pOldBuffer == NULL || pNewBuffer == NULL) {
		return PP_PARAM_ERROR;
	}

	if (ppC->hwId == 0x8170U) {
		PP_API_TRC("PPDecSwapLastOutputBuffer: Feature NOT suppported!\n");
		return PP_PARAM_ERROR;
	}

	if (ppC->decInst == NULL || (!ppC->multiBuffer)) {
		return PP_PARAM_ERROR;
	}

	if (pNewBuffer->bufferBusAddr == 0) {
		return PP_PARAM_ERROR;
	}

	if (PPGetStatus(ppC) != PP_STATUS_IDLE) {
		return PP_BUSY;
	}

	/* last multibuffer PP output */
	lastPPOut = &ppC->combinedModeBuffers.ppOutputBuffers[ppC->displayIndex];

	/* check that user swaps the last output */
	if ((pOldBuffer->bufferBusAddr != lastPPOut->bufferBusAddr) ||
		(pOldBuffer->bufferChromaBusAddr != lastPPOut->bufferChromaBusAddr)) {
		return PP_PARAM_ERROR;
	}

	lastPPOut->bufferBusAddr = pNewBuffer->bufferBusAddr;
	lastPPOut->bufferChromaBusAddr = pNewBuffer->bufferChromaBusAddr;

	PP_API_TRC("PPDecSwapLastOutputBuffer OK \n");
	return PP_OK;
}


/*------------------------------------------------------------------------------
    Function name   : PPDecSetMultipleOutput
    Description     :
    Return type     :
    Argument        :
    Argument        :
------------------------------------------------------------------------------*/
PPResult PPDecSetMultipleOutput(PPInst postPInst,
								const PPOutputBuffers *pBuffers)
{
	PPContainer *ppC;
	u32 i = 0;

	ppC = (PPContainer *) postPInst;

	PP_API_TRC("PPDecSetMultipleOutput\n");

	if (ppC == NULL || pBuffers == NULL) {
		return PP_PARAM_ERROR;
	}

	if (ppC->hwId == 0x8170U) {
		PP_API_TRC("PPDecSetMultipleOutput: Feature NOT suppported!\n");
		return PP_PARAM_ERROR;
	}

	if (ppC->decInst == NULL) {
		return PP_PARAM_ERROR;
	}

#ifdef PP_H264DEC_PIPELINE_SUPPORT
	/* multibuffer cannot be used if PP in combined mode with H264 decoder
	 * and decoder was initialized to use display smoothing */
	if (ppC->decType == PP_PIPELINED_DEC_TYPE_H264 &&
		h264UseDisplaySmoothing(ppC->decInst)) {
		return PP_PARAM_ERROR;
	}
#endif

	if (pBuffers->nbrOfBuffers == 0 ||
		pBuffers->nbrOfBuffers > PP_MAX_MULTIBUFFER) {
		return PP_PARAM_ERROR;
	}

	for (i = 0; i < pBuffers->nbrOfBuffers; i++)
		if (pBuffers->ppOutputBuffers[i].bufferBusAddr == 0) {
			return PP_PARAM_ERROR;
		}

	for (i = 0; i < pBuffers->nbrOfBuffers; i++) {
		ppC->bufferData[i].setupID = ppC->currentSetupID;
	}

	/* mark previously decoded pics dirty */
	ppC->currentSetupID++;

	(void) DWLmemcpy(&ppC->combinedModeBuffers, pBuffers,
					 sizeof(PPOutputBuffers));
	ppC->multiBuffer = 1;

	PP_API_TRC("PPDecSetMultipleOutput OK \n");
	return PP_OK;

}

/*------------------------------------------------------------------------------
    Function name   : PPGetNextOutput
    Description     : API function for acquiring pp output location
    Return type     : PPResult
    Argument        : PPInst postPInst
    Argument        :
------------------------------------------------------------------------------*/
PPResult PPGetNextOutput(PPInst postPInst, PPOutput *pOut)
{
	PPContainer *ppC;
	PPResult ret = PP_OK;

	ppC = (PPContainer *) postPInst;

	PP_API_TRC("PPGetNextOutput\n");

	if (ppC == NULL || pOut == NULL) {
		return PP_PARAM_ERROR;
	}

	if (ppC->hwId == 0x8170U) {
		PP_API_TRC("PPGetNextOutput: Feature NOT suppported!\n");
		return PP_PARAM_ERROR;
	}
	if (PPGetStatus(ppC) != PP_STATUS_IDLE) {
		return PP_BUSY;
	}

	if (ppC->multiBuffer) {
		u32 displayIndex = ppC->displayIndex;

		if (displayIndex >= ppC->combinedModeBuffers.nbrOfBuffers) {
			PP_API_TRC("PPGetNextOutput: Output index out of range!\n");
			return PP_DEC_RUNTIME_ERROR;
		}

		pOut->bufferBusAddr =
			ppC->combinedModeBuffers.ppOutputBuffers[displayIndex].
			bufferBusAddr;
		pOut->bufferChromaBusAddr =
			ppC->combinedModeBuffers.ppOutputBuffers[displayIndex].
			bufferChromaBusAddr;
		pOut->virtualAddress =
			ppC->combinedModeBuffers.ppOutputBuffers[displayIndex].
			virtualAddress;
		pOut->ChromaVirtualAddress =
			ppC->combinedModeBuffers.ppOutputBuffers[displayIndex].
			ChromaVirtualAddress;

		if (ppC->currentSetupID != ppC->bufferData[displayIndex].setupID) {
			u32 luma_size;
			/* the config has changed after processing this picture */
			/* The input source information was stored here previously, so rerun */
			SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_IN_LU_BASE,
							ppC->bufferData[displayIndex].inputBusLuma);
			SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_IN_CB_BASE,
							ppC->bufferData[displayIndex].inputBusChroma);

			/* Process bottom fields also */
			if (ppC->ppCfg.ppOutDeinterlace.enable &&
				!ppC->fastVerticalDownscale) {
				SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_BOT_YIN_BASE,
								ppC->bufferData[displayIndex].bottomBusLuma);
				SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_BOT_CIN_BASE,
								ppC->bufferData[displayIndex].bottomBusChroma);
			}

			if (ppC->ppCfg.ppOutFrmBuffer.frameBufferWidth &&
				ppC->ppCfg.ppOutFrmBuffer.frameBufferHeight) {
				luma_size = ((ppC->ppCfg.ppOutFrmBuffer.frameBufferWidth + 7) & ~7) *
							ppC->ppCfg.ppOutFrmBuffer.frameBufferHeight;
			} else {
				luma_size = ((ppC->ppCfg.ppOutImg.width + 7) & ~7) *
							ppC->ppCfg.ppOutImg.height;
			}
			/* update chroma address for PP.*/
			pOut->bufferChromaBusAddr = pOut->bufferBusAddr + luma_size;
			pOut->ChromaVirtualAddress = pOut->virtualAddress + luma_size / 4;
			/* output buffer setup */
			SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_OUT_LU_BASE,
							(pOut->bufferBusAddr +
							 ppC->frmBufferLumaOrRgbOffset));
			SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_OUT_CH_BASE,
							(pOut->bufferChromaBusAddr +
							 ppC->frmBufferChromaOffset));

			/* rerun picture because the configuration has changed */
			PP_API_TRC("PPGetNextOutput: Config changed... rerun\n");
			if (PPRun(ppC) != PP_OK) {
				return PP_BUSY;
			}
			ret = WaitForPp(ppC);
		}
	} else {
		pOut->bufferBusAddr = ppC->ppCfg.ppOutImg.bufferBusAddr;
		pOut->bufferChromaBusAddr = ppC->ppCfg.ppOutImg.bufferChromaBusAddr;
	}

	PP_API_TRC("PPGetNextOutput end\n");
	return ret;
}

#ifdef PP_DEC_PIPELINE_SUPPORT
/*------------------------------------------------------------------------------
    Function name   : PPDecSetOutBuffer
    Description     : set up multibuffer
    Return type     : void
    Argument        : container
------------------------------------------------------------------------------*/

#ifdef ASIC_TRACE_SUPPORT
extern u8 *pDisplayY;
extern u8 *pDisplayC;
#endif

static void PPDecSetOutBuffer(PPContainer *ppC, const DecPpInterface *decpp)
{

	PPOutImage *ppOutImg;

	ppOutImg = &ppC->ppCfg.ppOutImg;

	ASSERT(decpp->bufferIndex < ppC->combinedModeBuffers.nbrOfBuffers);

	ppOutImg->bufferBusAddr =
		ppC->combinedModeBuffers.ppOutputBuffers[decpp->bufferIndex].
		bufferBusAddr;

#if defined(PP_H264DEC_PIPELINE_SUPPORT) ||\
    defined(PP_VP8DEC_PIPELINE_SUPPORT)
	u32 luma_size;
	if (ppC->ppCfg.ppOutFrmBuffer.frameBufferWidth &&
		ppC->ppCfg.ppOutFrmBuffer.frameBufferHeight) {
		luma_size = ppC->ppCfg.ppOutFrmBuffer.frameBufferWidth *
					ppC->ppCfg.ppOutFrmBuffer.frameBufferHeight;
	} else {
		luma_size = ppC->ppCfg.ppOutImg.width *
					ppC->ppCfg.ppOutImg.height;
	}
	ppOutImg->bufferChromaBusAddr = ppOutImg->bufferBusAddr + luma_size;
#else
	ppOutImg->bufferChromaBusAddr =
		ppC->combinedModeBuffers.ppOutputBuffers[decpp->bufferIndex].
		bufferChromaBusAddr;
#endif
	ppC->displayIndex = decpp->displayIndex;

#ifdef ASIC_TRACE_SUPPORT
	pDisplayY = ppOutImg->bufferBusAddr;
	pDisplayC = ppOutImg->bufferChromaBusAddr;
#endif

	/* output buffer setup */
	SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_OUT_LU_BASE,
					(ppOutImg->bufferBusAddr +
					 ppC->frmBufferLumaOrRgbOffset));

	/* chromas not needed for RGB and YUYV 422 out */
	if (ppOutImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR) {
		SET_PP_ADDR_REG(ppC->ppRegs, HWIF_PP_OUT_CH_BASE,
						(ppOutImg->bufferChromaBusAddr +
						 ppC->frmBufferChromaOffset));
	}


	/* store some info if the config is changed and we need to rerun the pic */
	ppC->bufferData[decpp->bufferIndex].setupID = ppC->currentSetupID;

	/* Code below is required for H264 multibuffer support, but must not
	 * be used on any formats as they have different handling of PP input
	 * buffers. */
#ifdef PP_H264DEC_PIPELINE_SUPPORT
	ppC->bufferData[decpp->bufferIndex].inputBusLuma = decpp->inputBusLuma;
	ppC->bufferData[decpp->bufferIndex].inputBusChroma = decpp->inputBusChroma;
	ppC->bufferData[decpp->bufferIndex].bottomBusLuma = decpp->bottomBusLuma;
	ppC->bufferData[decpp->bufferIndex].bottomBusChroma = decpp->bottomBusChroma;
#endif /* PP_H264DEC_PIPELINE_SUPPORT */

}

/*------------------------------------------------------------------------------
    Function name   : PPDecDisplayIndex
    Description     : set next output in display order
    Return type     : void
    Argument        : container
------------------------------------------------------------------------------*/
#if defined(PP_MPEG4DEC_PIPELINE_SUPPORT) ||\
    defined(PP_H264DEC_PIPELINE_SUPPORT) ||\
    defined(PP_VC1DEC_PIPELINE_SUPPORT) ||\
    defined(PP_MPEG2DEC_PIPELINE_SUPPORT) ||\
    defined(PP_RVDEC_PIPELINE_SUPPORT) ||\
    defined(PP_AVSDEC_PIPELINE_SUPPORT) ||\
    defined(PP_VP8DEC_PIPELINE_SUPPORT) ||\
    defined(PP_WEBPDEC_PIPELINE_SUPPORT)
void PPDecDisplayIndex(const void *postPInst, u32 index)
{
	PPContainer *ppC;

	PP_API_TRC("PPDecDisplayIndex #");
	ppC = (PPContainer *) postPInst;

	ASSERT(index < ppC->combinedModeBuffers.nbrOfBuffers);

	ppC->displayIndex = index;

	return;
}
#endif

/*------------------------------------------------------------------------------
    Function name   : PPDecBufferData
    Description     : Store some info if the config is changed
    Return type     : void
    Argument        : container
------------------------------------------------------------------------------*/
#if defined(PP_MPEG4DEC_PIPELINE_SUPPORT) ||\
    defined(PP_VC1DEC_PIPELINE_SUPPORT) ||\
    defined(PP_MPEG2DEC_PIPELINE_SUPPORT) ||\
    defined(PP_RVDEC_PIPELINE_SUPPORT) ||\
    defined(PP_AVSDEC_PIPELINE_SUPPORT) ||\
    defined(PP_VP8DEC_PIPELINE_SUPPORT) ||\
    defined(PP_WEBPDEC_PIPELINE_SUPPORT)
void PPDecBufferData(const void *postPInst, u32 bufferIndex,
					 g1_addr_t inputBusLuma, g1_addr_t inputBusChroma,
					 g1_addr_t bottomBusLuma, g1_addr_t bottomBusChroma)
{
	PPContainer *ppC;

	PP_API_TRC("PPDecBufferData #");
	ppC = (PPContainer *) postPInst;

	ASSERT(bufferIndex < ppC->combinedModeBuffers.nbrOfBuffers);

	/* store some info if the config is changed and we need to rerun the pic */
	ppC->bufferData[bufferIndex].setupID = ppC->currentSetupID;
	ppC->bufferData[bufferIndex].inputBusLuma = inputBusLuma;
	ppC->bufferData[bufferIndex].inputBusChroma = inputBusChroma;
	ppC->bufferData[bufferIndex].bottomBusLuma = bottomBusLuma;
	ppC->bufferData[bufferIndex].bottomBusChroma = bottomBusChroma;

	return;
}
#endif

#if 0
/* find the buffer id in ppOutputBuffers[] according to address
   and return it to free. */
static u32 PPReturnMultiBufferId(const void *postPInst, g1_addr_t bus_addr)
{
	PPContainer *ppC = (PPContainer *)postPInst;
	u32 idx;
	PPOutput *pp_output;
	pthread_mutex_lock(&ppC->pp_buf_mutex);
	for (idx = 0; idx < ppC->combinedModeBuffers.nbrOfBuffers; idx++) {
		pp_output = &ppC->combinedModeBuffers.ppOutputBuffers[idx];
		if (bus_addr == pp_output->bufferBusAddr) {
			ASSERT(pp_output->BufUsed);
			ASSERT(pp_output->MemIdx == PP_INVALID_INDEX);
			break;
		}
	}
	ASSERT(idx < ppC->combinedModeBuffers.nbrOfBuffers);
	ppC->combinedModeBuffers.ppOutputBuffers[idx].BufUsed = 0;
	ppC->combinedModeBuffers.ppOutputBuffers[idx].MemIdx = PP_INVALID_INDEX;
	pthread_cond_signal(&ppC->pp_buf_release_cv);
	pthread_mutex_unlock(&ppC->pp_buf_mutex);

	return idx;
}

/* find a free buffer id in ppOutputBuffers[] */
static u32 PPGetFreeMultiBufferId(const void *postPInst, u32 mem_id)
{
	PPContainer *ppC = (PPContainer *)postPInst;
	u32 idx = 0;
	PPOutput *pp_output;

	pthread_mutex_lock(&ppC->pp_buf_mutex);
	do {
		idx = 0;
		for (idx = 0; idx < ppC->combinedModeBuffers.nbrOfBuffers; idx++) {
			pp_output = &ppC->combinedModeBuffers.ppOutputBuffers[idx];
			if (!pp_output->BufUsed &&
				pp_output->MemIdx == PP_INVALID_INDEX &&
				pp_output->bufferBusAddr != 0) {
				break;
			}
		}
		if (idx == ppC->combinedModeBuffers.nbrOfBuffers) {
			pthread_cond_wait(&ppC->pp_buf_release_cv, &ppC->pp_buf_mutex);
		}
	} while (idx == ppC->combinedModeBuffers.nbrOfBuffers);


	ASSERT(idx < ppC->combinedModeBuffers.nbrOfBuffers);

	pp_output->MemIdx = mem_id;
	pp_output->BufUsed = 1;
	pthread_mutex_unlock(&ppC->pp_buf_mutex);

	return idx;
}

/* find the buffer id in ppOutputBuffers[] according to MemIdx */
static u32 PPFindDisplayId(const void *postPInst, u32 mem_id, u32 is_release)
{
	PPContainer *ppC = (PPContainer *)postPInst;
	u32 idx;
	PPOutput *pp_output;
	pthread_mutex_lock(&ppC->pp_buf_mutex);
	for (idx = 0; idx < ppC->combinedModeBuffers.nbrOfBuffers; idx++) {
		pp_output = &ppC->combinedModeBuffers.ppOutputBuffers[idx];
		if (pp_output->MemIdx != PP_INVALID_INDEX &&
			mem_id == pp_output->MemIdx) {
			break;
		}
	}
	if (idx < ppC->combinedModeBuffers.nbrOfBuffers) {
		ASSERT(pp_output->BufUsed);
		pp_output->MemIdx = PP_INVALID_INDEX;
		if (is_release) {
			pp_output->BufUsed = 0;
			pthread_cond_signal(&ppC->pp_buf_release_cv);
		}
	}
	pthread_mutex_unlock(&ppC->pp_buf_mutex);
	ASSERT(idx < ppC->combinedModeBuffers.nbrOfBuffers);
	return idx;
}

static void PPRecoverMemInfo(const void *postPInst, u32 idx, u32 mem_id)
{
	PPContainer *ppC = (PPContainer *)postPInst;
	ASSERT(idx < ppC->combinedModeBuffers.nbrOfBuffers);
	PPOutput *pp_output;

	pp_output = &ppC->combinedModeBuffers.ppOutputBuffers[idx];
	pthread_mutex_lock(&ppC->pp_buf_mutex);
	pp_output->MemIdx = mem_id;
	pp_output->BufUsed = 1;
	pthread_mutex_unlock(&ppC->pp_buf_mutex);
}

static void PPResetMultiBuffer(const void *postPInst)
{
	int idx;
	PPContainer *ppC = (PPContainer *)postPInst;
	PPOutput *pp_output;
	pthread_mutex_lock(&ppC->pp_buf_mutex);
	for (idx = 0; idx < PP_MAX_MULTIBUFFER; idx++) {
		pp_output = &ppC->combinedModeBuffers.ppOutputBuffers[idx];
		/* those buffers are used for display and haven't been consumed. */
		while (pp_output->BufUsed && pp_output->MemIdx == PP_INVALID_INDEX) {
			pthread_cond_wait(&ppC->pp_buf_release_cv, &ppC->pp_buf_mutex);
		}
		pp_output->BufUsed = 0;
		pp_output->MemIdx = PP_INVALID_INDEX;
	}
	pthread_mutex_unlock(&ppC->pp_buf_mutex);
}

static void PPAddMultiBuffer(const void *postPInst, const void *mem_info)
{
	DWLLinearMem_t *info = (DWLLinearMem_t *)mem_info;
	PPContainer *ppC = (PPContainer *)postPInst;
	PPOutput *pp_output;
	u32 luma_size;

	ASSERT(ppC->multiBuffer);
	pthread_mutex_lock(&ppC->pp_buf_mutex);
	pp_output = &ppC->combinedModeBuffers.ppOutputBuffers[ppC->combinedModeBuffers.nbrOfBuffers];
	ASSERT(!pp_output->BufUsed);

	if (ppC->ppCfg.ppOutFrmBuffer.frameBufferWidth &&
		ppC->ppCfg.ppOutFrmBuffer.frameBufferHeight) {
		luma_size = ((ppC->ppCfg.ppOutFrmBuffer.frameBufferWidth + 7) & ~7) *
					ppC->ppCfg.ppOutFrmBuffer.frameBufferHeight;
	} else {
		luma_size = ((ppC->ppCfg.ppOutImg.width + 7) & ~7) *
					ppC->ppCfg.ppOutImg.height;
	}

	pp_output->virtualAddress = info->virtualAddress;
	pp_output->bufferBusAddr = info->busAddress;
	pp_output->ChromaVirtualAddress = info->virtualAddress + luma_size / 4;
	pp_output->bufferChromaBusAddr = info->busAddress + luma_size;
	pp_output->BufUsed = 0;
	pp_output->MemIdx = PP_INVALID_INDEX;
	ppC->combinedModeBuffers.nbrOfBuffers++;
	ASSERT(ppC->combinedModeBuffers.nbrOfBuffers <= PP_MAX_MULTIBUFFER);
	pthread_mutex_unlock(&ppC->pp_buf_mutex);
}

#endif

#endif /*PP_DEC_PIPELINE_SUPPORT */
