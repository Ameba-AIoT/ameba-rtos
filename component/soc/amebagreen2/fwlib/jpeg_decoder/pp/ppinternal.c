/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*------------------------------------------------------------------------------
    1.  Include headers
------------------------------------------------------------------------------*/

#include "basetype.h"
#include "ppapi.h"
#include "ppinternal.h"
#include "dwl.h"
#include "regdrv.h"
#include "ppdebug.h"
#include "ppcfg.h"
#include "decapicommon.h"

static void PPSetFrmBufferWriting(PPContainer *ppC);
static void PPSetRgbBitmask(PPContainer *ppC);
static void PPSetRgbTransformCoeffs(PPContainer *ppC);
static void PPSetDithering(PPContainer *ppC);

static u32 PPIsInPixFmtOk(u32 pix_fmt, const PPContainer *ppC);
static u32 PPIsOutPixFmtOk(u32 pix_fmt, const PPContainer *ppC);

static i32 PPCheckAllWidthParams(PPConfig *ppCfg, u32 blendEna, u32 pixAcc,
								 u32 ablendCrop);
static i32 PPCheckAllHeightParams(PPConfig *ppCfg, u32 pixAcc);

static u32 PPFindFirstNonZeroBit(u32 mask);
static void PPSetRgbBitmaskCustom(PPContainer *ppC, u32 rgb16);

static i32 PPContinuousCheck(u32 value);
static i32 PPCheckOverlapping(u32 a, u32 b, u32 c, u32 d);

static u32 PPCountOnes(u32 value);
static u32 PPSelectDitheringValue(u32 mask);

#if (PP_X170_DATA_BUS_WIDTH != 4) && (PP_X170_DATA_BUS_WIDTH != 8)
#error "Bad data bus width specified PP_X170_DATA_BUS_WIDTH"
#endif

#define PP_FAST_SCALING_UNINITIALIZED   (0)
#define PP_FAST_SCALING_SUPPORTED       (1)
#define PP_FAST_SCALING_NOT_SUPPORTED   (2)

/*------------------------------------------------------------------------------
    Function name   : PPGetStatus
    Description     :
    Return type     : u32
    Argument        : PPContainer *ppC
------------------------------------------------------------------------------*/
u32 PPGetStatus(const PPContainer *ppC)
{
	ASSERT(ppC != NULL);
	return ppC->status;
}

/*------------------------------------------------------------------------------
    Function name   : PPSetStatus
    Description     :
    Return type     : void
    Argument        : PPContainer *ppC
    Argument        : u32 status
------------------------------------------------------------------------------*/
void PPSetStatus(PPContainer *ppC, u32 status)
{
	ASSERT(ppC != NULL);
	ppC->status = status;
}

/*------------------------------------------------------------------------------
    Function name   : PPRefreshRegs
    Description     :
    Return type     : void
    Argument        : PPContainer * ppC
------------------------------------------------------------------------------*/
void PPRefreshRegs(PPContainer *ppC)
{
	i32 i;
	u32 offset = PP_X170_REG_START;

	u32 *ppRegs = ppC->ppRegs;

	for (i = PP_X170_REGISTERS; i > 0; i--) {
		*ppRegs++ = DWLReadReg(ppC->dwl, ppC->coreID, offset);
		offset += 4;
	}
#ifdef USE_64BIT_ENV
#ifndef VERIFY_32BIT_NANOD_IN_64BIT_ENV
	offset = PP_X170_EXPAND_REG_START;
	ppRegs = ppC->ppRegs + ((PP_X170_EXPAND_REG_START - PP_X170_REG_START) >> 2);
	for (i = PP_X170_EXPAND_REGS; i > 0; i--) {
		*ppRegs++ = DWLReadReg(ppC->dwl, ppC->coreID, offset);
		offset += 4;
	}
#endif
#endif
}

/*------------------------------------------------------------------------------
    Function name   : PPFlushRegs
    Description     :
    Return type     : void
    Argument        : PPContainer * ppC
------------------------------------------------------------------------------*/
void PPFlushRegs(PPContainer *ppC)
{
	i32 i;
	u32 offset = PP_X170_REG_START;
	u32 *ppRegs = ppC->ppRegs;

	for (i = PP_X170_REGISTERS; i > 0; i--) {
		DWLWriteReg(ppC->dwl, ppC->coreID, offset, *ppRegs);
		ppRegs++;
		offset += 4;
	}
	DWLWriteReg(ppC->dwl, ppC->coreID, 77 * 4, ppC->ppRegs[77]);
#ifdef USE_64BIT_ENV
#ifndef VERIFY_32BIT_NANOD_IN_64BIT_ENV
	offset = PP_X170_EXPAND_REG_START;
	ppRegs = ppC->ppRegs + ((PP_X170_EXPAND_REG_START - PP_X170_REG_START) >> 2) ;
	for (i = PP_X170_EXPAND_REGS; i > 0; i--) {
		DWLWriteReg(ppC->dwl, ppC->coreID, offset, *ppRegs);
		ppRegs++;
		offset += 4;
	}
#endif
#endif
}

/*------------------------------------------------------------------------------
    Function name   : PPInitHW
    Description     :
    Return type     : void
    Argument        : PPContainer * ppC
------------------------------------------------------------------------------*/
void PPInitHW(PPContainer *ppC)
{
	DWLHwConfig_t hwCfg;
	u32 asicId;
	u32 *ppRegs = ppC->ppRegs;

	(void) DWLmemset(ppRegs, 0, TOTAL_X170_REGISTERS * sizeof(*ppRegs));

#if( PP_X170_USING_IRQ == 0 )
	SetPpRegister(ppRegs, HWIF_PP_IRQ_DIS, 1);
#endif

#if (PP_X170_OUTPUT_PICTURE_ENDIAN > 1)
#error "Bad value specified for PP_X170_OUTPUT_PICTURE_ENDIAN"
#endif

#if (PP_X170_INPUT_PICTURE_ENDIAN > 1)
#error "Bad value specified for PP_X170_INPUT_PICTURE_ENDIAN"
#endif

#if (PP_X170_BUS_BURST_LENGTH > 31)
#error "Bad value specified for PP_X170_BUS_BURST_LENGTH"
#endif

	SetPpRegister(ppRegs, HWIF_PP_IN_ENDIAN, PP_X170_INPUT_PICTURE_ENDIAN);
	SetPpRegister(ppRegs, HWIF_PP_MAX_BURST, PP_X170_BUS_BURST_LENGTH);

	DWLReadAsicConfig(&hwCfg);

	/* Check to see if HW supports extended endianess modes */
	ppC->hwEndianVer = (hwCfg.ppConfig & PP_OUTP_ENDIAN) ? 1 : 0;
	ppC->tiledModeSupport = (hwCfg.ppConfig & PP_TILED_INPUT) >> 14;
	ppC->jpeg16kSupport = 1; /* support 16K size */

	asicId = DWLReadAsicID();
	if ((asicId & ~0x7) == 0x67311148) { /* Workaround needed for this HW build */
		ppC->cHworkaroundFlag = 1;
	} else {
		ppC->cHworkaroundFlag = 0;
	}

#if ( PP_X170_DATA_DISCARD_ENABLE != 0 )
	SetPpRegister(ppRegs, HWIF_PP_DATA_DISC_E, 1);
#else
	SetPpRegister(ppRegs, HWIF_PP_DATA_DISC_E, 0);
#endif

	/* Note; output endianess settings moved to PPSetupHW(), these left
	 * here as reminder and placeholder for possible future updates */
	/*
	    SetPpRegister(ppRegs, HWIF_PP_OUT_ENDIAN, PP_X170_OUTPUT_PICTURE_ENDIAN);

	#if ( PP_X170_SWAP_32_WORDS != 0 )
	    SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP32_E, 1);
	#else
	    SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP32_E, 0);
	#endif

	    if( ppC->hwEndianVer )
	    {
	#if ( PP_X170_SWAP_16_WORDS != 0 )
	        SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP16_E, 1);
	#else
	        SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP16_E, 0);
	#endif
	    }
	    */

#if ( PP_X170_SWAP_32_WORDS_INPUT != 0 )
	SetPpRegister(ppRegs, HWIF_PP_IN_SWAP32_E, 1);
	SetPpRegister(ppRegs, HWIF_PP_IN_A1_SWAP32, 1);
#else
	SetPpRegister(ppRegs, HWIF_PP_IN_SWAP32_E, 0);
	SetPpRegister(ppRegs, HWIF_PP_IN_A1_SWAP32, 0);
#endif

#if ( PP_X170_INTERNAL_CLOCK_GATING != 0 )
	SetPpRegister(ppRegs, HWIF_PP_CLK_GATE_E, 1);
#else
	SetPpRegister(ppRegs, HWIF_PP_CLK_GATE_E, 0);
#endif

	/* set AXI RW IDs */
	SetPpRegister(ppRegs, HWIF_PP_AXI_RD_ID, (PP_X170_AXI_ID_R & 0xFFU));
	SetPpRegister(ppRegs, HWIF_PP_AXI_WR_ID, (PP_X170_AXI_ID_W & 0xFFU));

	SetPpRegister(ppRegs, HWIF_PP_SCMD_DIS, PP_X170_SCMD_DISABLE);

	/* use alpha blend source 1 endian mode for alpha blend source 2 */
	SetPpRegister(ppC->ppRegs, HWIF_PP_IN_A2_ENDSEL, 1);


	return;
}

/*------------------------------------------------------------------------------
    Function name   : PPInitDataStructures
    Description     :
    Return type     : void
    Argument        : PPContainer * ppC
------------------------------------------------------------------------------*/
void PPInitDataStructures(PPContainer *ppC)
{
	PPOutImage *ppOutImg;
	PPInImage *ppInImg;

	PPOutRgb *ppOutRgb;

	ASSERT(ppC != NULL);

	(void) DWLmemset(&ppC->ppCfg, 0, sizeof(PPConfig));

	ppOutImg = &ppC->ppCfg.ppOutImg;
	ppInImg = &ppC->ppCfg.ppInImg;

	ppOutRgb = &ppC->ppCfg.ppOutRgb;

	ppInImg->width = 720;
	ppInImg->height = 576;
	ppInImg->pixFormat = PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR;

	ppC->inFormat = PP_ASIC_IN_FORMAT_420_SEMIPLANAR;

	ppOutImg->width = 720;
	ppOutImg->height = 576;
	ppOutImg->pixFormat = PP_PIX_FMT_RGB32;

	ppC->outFormat = PP_ASIC_OUT_FORMAT_RGB;
	ppC->rgbDepth = 32;

	ppOutRgb->rgbTransform = PP_YCBCR2RGB_TRANSFORM_BT_601;

	ppOutRgb->rgbTransformCoeffs.a = 298;
	ppOutRgb->rgbTransformCoeffs.b = 409;
	ppOutRgb->rgbTransformCoeffs.c = 208;
	ppOutRgb->rgbTransformCoeffs.d = 100;
	ppOutRgb->rgbTransformCoeffs.e = 516;

	ppC->frmBufferLumaOrRgbOffset = 0;
	ppC->frmBufferChromaOffset = 0;

}

/*------------------------------------------------------------------------------
    Function name   : PPSetupHW
    Description     :
    Return type     : void
    Argument        : PPContainer * ppC
------------------------------------------------------------------------------*/
void PPSetupHW(PPContainer *ppC)
{

	PPOutMask1 *ppOutMask1;
	PPOutMask2 *ppOutMask2;
	PPOutImage *ppOutImg;
	PPInImage *ppInImg;
	PPInCropping *ppInCrop;
	PPOutDeinterlace *ppOutDeint;
	PPOutRgb *ppOutRgb;

	PPInRotation *ppInRot;

	u32 *ppRegs;

	ASSERT(ppC != NULL);

	ppOutMask1 = &ppC->ppCfg.ppOutMask1;
	ppOutMask2 = &ppC->ppCfg.ppOutMask2;
	ppOutImg = &ppC->ppCfg.ppOutImg;
	ppInImg = &ppC->ppCfg.ppInImg;
	ppInCrop = &ppC->ppCfg.ppInCrop;
	ppOutDeint = &ppC->ppCfg.ppOutDeinterlace;

	ppInRot = &ppC->ppCfg.ppInRotation;
	ppOutRgb = &ppC->ppCfg.ppOutRgb;

	ppRegs = ppC->ppRegs;
	/* frame buffer setup */
	PPSetFrmBufferWriting(ppC);

	/* setup output endianess */
	if ((ppC->outFormat == PP_ASIC_OUT_FORMAT_RGB) &&
		ppC->hwEndianVer) {
		u32 isRgb16 = (ppC->rgbDepth == 16);
		SetPpRegister(ppRegs, HWIF_PP_OUT_ENDIAN,
					  isRgb16 ? PP_X170_OUTPUT_PICTURE_ENDIAN_RGB16 :
					  PP_X170_OUTPUT_PICTURE_ENDIAN_RGB32);

		if (isRgb16) {
#if ( PP_X170_SWAP_32_WORDS_RGB16 != 0 )
			SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP32_E, 1);
#else
			SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP32_E, 0);
#endif

#if ( PP_X170_SWAP_16_WORDS_RGB16 != 0 )
			SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP16_E, 1);
#else
			SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP16_E, 0);
#endif
		} else {
#if ( PP_X170_SWAP_32_WORDS_RGB32 != 0 )
			SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP32_E, 1);
#else
			SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP32_E, 0);
#endif

#if ( PP_X170_SWAP_16_WORDS_RGB32 != 0 )
			SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP16_E, 1);
#else
			SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP16_E, 0);
#endif
		}
	} else { /* Output not RGB, or no extended endian support */
		SetPpRegister(ppRegs, HWIF_PP_OUT_ENDIAN, PP_X170_OUTPUT_PICTURE_ENDIAN);

#if ( PP_X170_SWAP_32_WORDS != 0 )
		SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP32_E, 1);
#else
		SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP32_E, 0);
#endif

		if (ppC->hwEndianVer) {
#if ( PP_X170_SWAP_16_WORDS != 0 )
			SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP16_E, 1);
#else
			SetPpRegister(ppRegs, HWIF_PP_OUT_SWAP16_E, 0);
#endif
		}
	}

	/* output buffer setup */
	SET_PP_ADDR_REG(ppRegs, HWIF_PP_OUT_LU_BASE,
					(ppOutImg->bufferBusAddr +
					 ppC->frmBufferLumaOrRgbOffset));

	/* chromas not needed for RGB and YUYV 422 out */
	if (ppOutImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR) {
		SET_PP_ADDR_REG(ppRegs, HWIF_PP_OUT_CH_BASE,
						(ppOutImg->bufferChromaBusAddr +
						 ppC->frmBufferChromaOffset));
	}

	SetPpRegister(ppRegs, HWIF_PP_OUT_FORMAT, ppC->outFormat);
	if (ppC->outFormat == PP_ASIC_OUT_FORMAT_422) {
		SetPpRegister(ppRegs, HWIF_PP_OUT_START_CH, ppC->outStartCh);
		SetPpRegister(ppRegs, HWIF_PP_OUT_CR_FIRST, ppC->outCrFirst);
		SetPpRegister(ppRegs, HWIF_PP_OUT_TILED_E, ppC->outTiled4x4);
	}
	SetPpRegister(ppRegs, HWIF_PP_OUT_WIDTH, ppOutImg->width);
	SetPpRegister(ppRegs, HWIF_PP_OUT_HEIGHT, ppOutImg->height);
	SetPpRegister(ppRegs, HWIF_PP_OUT_W_EXT, ppOutImg->width >> 11);
	SetPpRegister(ppRegs, HWIF_PP_OUT_H_EXT, ppOutImg->height >> 11);

	/* deinterlacing parameters */
	SetPpRegister(ppRegs, HWIF_DEINT_E, ppOutDeint->enable);

	if (ppOutDeint->enable) {
		/* deinterlacing default parameters */
		SetPpRegister(ppRegs, HWIF_DEINT_BLEND_E, 0);
		SetPpRegister(ppRegs, HWIF_DEINT_THRESHOLD, 25);
		SetPpRegister(ppRegs, HWIF_DEINT_EDGE_DET, 25);
	}

	/* input setup */
	if (ppC->decInst == NULL) {
		SetPpRegister(ppRegs, HWIF_PP_IN_STRUCT, ppInImg->picStruct);

		if (ppInImg->picStruct != PP_PIC_BOT_FIELD &&
			ppInImg->picStruct != PP_PIC_BOT_FIELD_FRAME) {
			SET_PP_ADDR_REG(ppRegs, HWIF_PP_IN_LU_BASE, ppInImg->bufferBusAddr);
			SET_PP_ADDR_REG(ppRegs, HWIF_PP_IN_CB_BASE, ppInImg->bufferCbBusAddr);
		}

		if (ppInImg->picStruct != PP_PIC_FRAME_OR_TOP_FIELD &&
			ppInImg->picStruct != PP_PIC_TOP_FIELD_FRAME) {
			SET_PP_ADDR_REG(ppRegs, HWIF_PP_BOT_YIN_BASE,
							ppInImg->bufferBusAddrBot);
			SET_PP_ADDR_REG(ppRegs, HWIF_PP_BOT_CIN_BASE,
							ppInImg->bufferBusAddrChBot);
		}

		if (ppInImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_0_PLANAR) {
			SET_PP_ADDR_REG(ppRegs, HWIF_PP_IN_CR_BASE, ppInImg->bufferCrBusAddr);
		}
	}

	SetPpRegister(ppRegs, HWIF_EXT_ORIG_WIDTH, (ppInImg->width + 15) / 16);
	if (ppC->inFormat < PP_ASIC_IN_FORMAT_EXTENSION) {
		SetPpRegister(ppRegs, HWIF_PP_IN_FORMAT, ppC->inFormat);
	} else {
		SetPpRegister(ppRegs, HWIF_PP_IN_FORMAT, PP_ASIC_IN_FORMAT_EXTENSION);
		SetPpRegister(ppRegs, HWIF_PP_IN_FORMAT_ES,
					  ppC->inFormat - PP_ASIC_IN_FORMAT_EXTENSION);
	}

	if (ppC->inFormat == PP_ASIC_IN_FORMAT_422) {
		SetPpRegister(ppRegs, HWIF_PP_IN_START_CH, ppC->inStartCh);
		SetPpRegister(ppRegs, HWIF_PP_IN_CR_FIRST, ppC->inCrFirst);
	}

	if (!ppInCrop->enable) {
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_W_EXT,
					  ((((ppInImg->width  + 15) / 16) & 0xE00) >> 9));
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_WIDTH,
					  (((ppInImg->width  + 15) / 16) & 0x1FF));
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_H_EXT,
					  ((((ppInImg->height + 15) / 16) & 0x700) >> 8));
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_HEIGHT,
					  (((ppInImg->height + 15) / 16) & 0x0FF));
		SetPpRegister(ppRegs, HWIF_CROP_STARTX_EXT, 0);
		SetPpRegister(ppRegs, HWIF_CROP_STARTX, 0);
		SetPpRegister(ppRegs, HWIF_CROP_STARTY_EXT, 0);
		SetPpRegister(ppRegs, HWIF_CROP_STARTY, 0);
		SetPpRegister(ppRegs, HWIF_PP_CROP8_R_E, 0);

		ppC->inWidth = ppInImg->width;
		ppC->inHeight = ppInImg->height;
	} else {
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_W_EXT,
					  ((((ppInCrop->width + 15) / 16) & 0xE00) >> 9));
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_WIDTH,
					  (((ppInCrop->width + 15) / 16) & 0x1FF));
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_H_EXT,
					  ((((ppInCrop->height + 15) / 16) & 0x700) >> 8));
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_HEIGHT,
					  (((ppInCrop->height + 15) / 16) & 0x0FF));
		SetPpRegister(ppRegs, HWIF_CROP_STARTX_EXT,
					  (((ppInCrop->originX / 16) & 0xE00) >> 9));
		SetPpRegister(ppRegs, HWIF_CROP_STARTX,
					  ((ppInCrop->originX / 16) & 0x1FF));
		SetPpRegister(ppRegs, HWIF_CROP_STARTY_EXT,
					  (((ppInCrop->originY / 16) & 0x700) >> 8));
		SetPpRegister(ppRegs, HWIF_CROP_STARTY,
					  ((ppInCrop->originY / 16) & 0x0FF));

		if (ppInCrop->width & 0x0F) {
			SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_R_E, 1);
		} else {
			SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_R_E, 0);
		}

		if (ppInCrop->height & 0x0F) {
			SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_D_E, 1);
		} else {
			SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_D_E, 0);
		}

		ppC->inWidth = ppInCrop->width;
		ppC->inHeight = ppInCrop->height;
	}

	/* setup scaling */
	PPSetupScaling(ppC, ppOutImg);

	/* YUV range */
	SetPpRegister(ppRegs, HWIF_YCBCR_RANGE, ppInImg->videoRange);

	if (ppOutImg->pixFormat & PP_PIXEL_FORMAT_RGB_MASK) {
		/* setup RGB conversion */
		PPSetRgbTransformCoeffs(ppC);

		if (ppOutRgb->ditheringEnable) {
			PPSetDithering(ppC);
		}
		/* setup RGB bitmasks */
		PPSetRgbBitmask(ppC);

	}

	if (ppC->decInst == NULL) {
		/* set up range expansion/mapping */

		SetPpRegister(ppC->ppRegs, HWIF_PP_VC1_ADV_E,
					  ppInImg->vc1AdvancedProfile);

		if (ppInImg->vc1RangeRedFrm) {
			SetPpRegister(ppC->ppRegs, HWIF_RANGEMAP_Y_E, 1);
			SetPpRegister(ppC->ppRegs, HWIF_RANGEMAP_COEF_Y, 7 + 9);
			SetPpRegister(ppC->ppRegs, HWIF_RANGEMAP_C_E, 1);
			SetPpRegister(ppC->ppRegs, HWIF_RANGEMAP_COEF_C, 7 + 9);
		} else {
			SetPpRegister(ppC->ppRegs, HWIF_RANGEMAP_Y_E,
						  ppInImg->vc1RangeMapYEnable);
			SetPpRegister(ppC->ppRegs, HWIF_RANGEMAP_COEF_Y,
						  ppInImg->vc1RangeMapYCoeff + 9);
			SetPpRegister(ppC->ppRegs, HWIF_RANGEMAP_C_E,
						  ppInImg->vc1RangeMapCEnable);
			SetPpRegister(ppC->ppRegs, HWIF_RANGEMAP_COEF_C,
						  ppInImg->vc1RangeMapCCoeff + 9);
		}
		/* for pipeline, this is set up in PipelineStart */
	}

	/* setup rotation/flip */
	SetPpRegister(ppRegs, HWIF_ROTATION_MODE, ppInRot->rotation);

	/* setup masks */
	SetPpRegister(ppRegs, HWIF_MASK1_E, ppOutMask1->enable);

	/* Alpha blending mask 1 */
	if (ppOutMask1->enable && ppOutMask1->alphaBlendEna && ppC->blendEna) {
		SetPpRegister(ppRegs, HWIF_MASK1_ABLEND_E, 1);
		if (ppC->blendCropSupport) {
			g1_addr_t blendComponentBase = ppOutMask1->blendComponentBase;
			blendComponentBase += 4 * (
									  ppOutMask1->blendOriginX +
									  ppOutMask1->blendOriginY * ppOutMask1->blendWidth);

			SET_PP_ADDR_REG(ppRegs, HWIF_ABLEND1_BASE, blendComponentBase);
			SetPpRegister(ppRegs, HWIF_ABLEND1_SCANL, ppOutMask1->blendWidth);

		} else { /* legacy mode */
			SET_PP_ADDR_REG(ppRegs, HWIF_ABLEND1_BASE,
							ppOutMask1->blendComponentBase);
		}
	} else {
		SetPpRegister(ppRegs, HWIF_MASK1_ABLEND_E, 0);
	}

	if (ppOutMask1->enable) {
		u32 startX, startY;
		i32 endX, endY;

		if (ppOutMask1->originX < 0) {
			startX = 0;
		} else if (ppOutMask1->originX > (i32) ppOutImg->width) {
			startX = ppOutImg->width;
		} else {
			startX = (u32) ppOutMask1->originX;
		}

		SetPpRegister(ppRegs, HWIF_MASK1_STARTX, startX);
		SetPpRegister(ppRegs, HWIF_MASK1_STARTX_EXT, startX >> 11);

		if (ppOutMask1->originY < 0) {
			startY = 0;
		} else if (ppOutMask1->originY > (i32) ppOutImg->height) {
			startY = ppOutImg->height;
		} else {
			startY = (u32) ppOutMask1->originY;
		}

		SetPpRegister(ppRegs, HWIF_MASK1_STARTY, startY);
		SetPpRegister(ppRegs, HWIF_MASK1_STARTY_EXT, startY >> 11);

		endX = ppOutMask1->originX + (i32) ppOutMask1->width;
		if (endX > (i32) ppOutImg->width) {
			endX = (i32) ppOutImg->width;
		} else if (endX < 0) {
			endX = 0;
		}

		SetPpRegister(ppRegs, HWIF_MASK1_ENDX, (u32) endX);
		SetPpRegister(ppRegs, HWIF_MASK1_ENDX_EXT, (u32) endX >> 11);

		endY = ppOutMask1->originY + (i32) ppOutMask1->height;
		if (endY > (i32) ppOutImg->height) {
			endY = (i32) ppOutImg->height;
		} else if (endY < 0) {
			endY = 0;
		}

		SetPpRegister(ppRegs, HWIF_MASK1_ENDY, (u32) endY);
		SetPpRegister(ppRegs, HWIF_MASK1_ENDY_EXT, (u32) endY >> 11);
	}

	SetPpRegister(ppRegs, HWIF_MASK2_E, ppOutMask2->enable);

	/* Alpha blending mask 2 */
	if (ppOutMask2->enable && ppOutMask2->alphaBlendEna && ppC->blendEna) {
		SetPpRegister(ppRegs, HWIF_MASK2_ABLEND_E, 1);
		if (ppC->blendCropSupport) {
			g1_addr_t blendComponentBase = ppOutMask2->blendComponentBase;
			blendComponentBase += 4 * (
									  ppOutMask2->blendOriginX +
									  ppOutMask2->blendOriginY * ppOutMask2->blendWidth);

			SET_PP_ADDR_REG(ppRegs, HWIF_ABLEND2_BASE, blendComponentBase);
			SetPpRegister(ppRegs, HWIF_ABLEND2_SCANL, ppOutMask2->blendWidth);

		} else { /* legacy mode */
			SET_PP_ADDR_REG(ppRegs, HWIF_ABLEND2_BASE,
							ppOutMask2->blendComponentBase);
		}
	} else {
		SetPpRegister(ppRegs, HWIF_MASK2_ABLEND_E, 0);
	}

	if (ppOutMask2->enable) {
		u32 startX, startY;
		i32 endX, endY;

		if (ppOutMask2->originX < 0) {
			startX = 0;
		} else if (ppOutMask2->originX > (i32) ppOutImg->width) {
			startX = ppOutImg->width;
		} else {
			startX = (u32) ppOutMask2->originX;
		}

		SetPpRegister(ppRegs, HWIF_MASK2_STARTX, startX);
		SetPpRegister(ppRegs, HWIF_MASK2_STARTX_EXT, startX >> 11);

		if (ppOutMask2->originY < 0) {
			startY = 0;
		} else if (ppOutMask2->originY > (i32) ppOutImg->height) {
			startY = ppOutImg->height;
		} else {
			startY = (u32) ppOutMask2->originY;
		}

		SetPpRegister(ppRegs, HWIF_MASK2_STARTY, startY);
		SetPpRegister(ppRegs, HWIF_MASK2_STARTY_EXT, startY >> 11);

		endX = ppOutMask2->originX + (i32) ppOutMask2->width;
		if (endX > (i32) ppOutImg->width) {
			endX = (i32) ppOutImg->width;
		} else if (endX < 0) {
			endX = 0;
		}

		SetPpRegister(ppRegs, HWIF_MASK2_ENDX, (u32) endX);
		SetPpRegister(ppRegs, HWIF_MASK2_ENDX_EXT, (u32) endX >> 11);

		endY = ppOutMask2->originY + (i32) ppOutMask2->height;
		if (endY > (i32) ppOutImg->height) {
			endY = (i32) ppOutImg->height;
		} else if (endY < 0) {
			endY = 0;
		}

		SetPpRegister(ppRegs, HWIF_MASK2_ENDY, (u32) endY);
		SetPpRegister(ppRegs, HWIF_MASK2_ENDY_EXT, (u32) endY >> 11);
	}
}

/*------------------------------------------------------------------------------
    Function name   : PPCheckTiledOutput
    Description     :
    Return type     : i32
    Argument        : PPConfig *ppCfg
------------------------------------------------------------------------------*/
i32 PPCheckTiledOutput(PPConfig *ppCfg)
{
	i32 ret = PP_OK;
	PPOutImage *ppOutImg;
	PPOutFrameBuffer *ppOutFrmBuffer;
	PPOutMask1 *ppOutMask1;
	PPOutMask2 *ppOutMask2;

	u32 mask = 0x3; /* fixed for 4x4 blocks */

	ppOutImg = &ppCfg->ppOutImg;
	ppOutFrmBuffer = &ppCfg->ppOutFrmBuffer;
	ppOutMask1 = &ppCfg->ppOutMask1;
	ppOutMask2 = &ppCfg->ppOutMask2;

	/* Assert output image format */
	if (ppOutImg->pixFormat != PP_PIX_FMT_YCBCR_4_2_2_TILED_4X4 &&
		ppOutImg->pixFormat != PP_PIX_FMT_YCRYCB_4_2_2_TILED_4X4 &&
		ppOutImg->pixFormat != PP_PIX_FMT_CBYCRY_4_2_2_TILED_4X4 &&
		ppOutImg->pixFormat != PP_PIX_FMT_CRYCBY_4_2_2_TILED_4X4) {
		ret = (i32) PP_SET_OUT_FORMAT_INVALID;
	}

	/* All scaling boundaries must be 4x4 */
	if (ppOutImg->width & mask) {
		ret = (i32) PP_SET_OUT_SIZE_INVALID;
	}
	if (ppOutImg->height & mask) {
		ret = (i32) PP_SET_OUT_SIZE_INVALID;
	}

	/* All mask boundaries must be 4x4 */
	if (ppOutMask1->enable) {
		if (ppOutMask1->width & mask) {
			ret = (i32) PP_SET_MASK1_INVALID;
		}
		if (ppOutMask1->originX & mask) {
			ret = (i32) PP_SET_MASK1_INVALID;
		}
		if (ppOutMask1->height & mask) {
			ret = (i32) PP_SET_MASK1_INVALID;
		}
		if (ppOutMask1->originY & mask) {
			ret = (i32) PP_SET_MASK1_INVALID;
		}
	}
	if (ppOutMask2->enable) {
		if (ppOutMask2->width & mask) {
			ret = (i32) PP_SET_MASK2_INVALID;
		}
		if (ppOutMask2->originX & mask) {
			ret = (i32) PP_SET_MASK2_INVALID;
		}
		if (ppOutMask2->height & mask) {
			ret = (i32) PP_SET_MASK2_INVALID;
		}
		if (ppOutMask2->originY & mask) {
			ret = (i32) PP_SET_MASK2_INVALID;
		}
	}

	/* Frame buffer mustn't be specified */
	if (ppOutFrmBuffer->enable) {
		ret = (i32) PP_SET_FRAMEBUFFER_INVALID;
	}

	return ret;

}

/*------------------------------------------------------------------------------
    Function name   : PPCheckConfig
    Description     :
    Return type     : i32
    Argument        : PPConfig * ppCfg
    Argument        : u32 pipeline
    Argument        : u32 decType
------------------------------------------------------------------------------*/
i32 PPCheckConfig(PPContainer *ppC, PPConfig *ppCfg,
				  u32 decLinked, u32 decType)
{
	PPOutImage *ppOutImg;
	PPInImage *ppInImg;
	PPInCropping *ppInCrop;
	PPOutRgb *ppOutRgb;
	PPOutFrameBuffer *ppOutFrmBuffer;
	PPInRotation *ppInRotation;
	PPOutDeinterlace *ppOutDeint;

	PPOutMask1 *ppOutMask1;
	PPOutMask2 *ppOutMask2;

	const u32 address_mask = (PP_X170_DATA_BUS_WIDTH - 1);

	ASSERT(ppCfg != NULL);

	ppOutImg = &ppCfg->ppOutImg;
	ppInImg = &ppCfg->ppInImg;
	ppInCrop = &ppCfg->ppInCrop;
	ppOutRgb = &ppCfg->ppOutRgb;
	ppOutFrmBuffer = &ppCfg->ppOutFrmBuffer;
	ppInRotation = &ppCfg->ppInRotation;
	ppOutDeint = &ppCfg->ppOutDeinterlace;

	ppOutMask1 = &ppCfg->ppOutMask1;
	ppOutMask2 = &ppCfg->ppOutMask2;

	/* PPInImage check */

	if (!PPIsInPixFmtOk(ppInImg->pixFormat, ppC)) {
		return (i32) PP_SET_IN_FORMAT_INVALID;
	}

	if (!decLinked) {
		if (ppInImg->picStruct != PP_PIC_BOT_FIELD &&
			ppInImg->picStruct != PP_PIC_BOT_FIELD_FRAME) {
			if ((ppInImg->bufferBusAddr == 0) ||
				(ppInImg->bufferBusAddr & address_mask)) {
				return (i32) PP_SET_IN_ADDRESS_INVALID;
			}

			if (ppInImg->pixFormat & PP_PIXEL_FORMAT_YUV420_MASK) {
				if (ppInImg->bufferCbBusAddr == 0 ||
					(ppInImg->bufferCbBusAddr & address_mask)) {
					return (i32) PP_SET_IN_ADDRESS_INVALID;
				}
			}

			if (ppInImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_0_PLANAR) {
				if (ppInImg->bufferCrBusAddr == 0 ||
					(ppInImg->bufferCrBusAddr & address_mask)) {
					return (i32) PP_SET_IN_ADDRESS_INVALID;
				}
			}
		}
		if (ppInImg->picStruct != PP_PIC_FRAME_OR_TOP_FIELD &&
			ppInImg->picStruct != PP_PIC_TOP_FIELD_FRAME) {
			if ((ppInImg->bufferBusAddrBot == 0) ||
				(ppInImg->bufferBusAddrBot & address_mask)) {
				return (i32) PP_SET_IN_ADDRESS_INVALID;
			}

			if (ppInImg->pixFormat & PP_PIXEL_FORMAT_YUV420_MASK) {
				if (ppInImg->bufferBusAddrChBot == 0 ||
					(ppInImg->bufferBusAddrChBot & address_mask)) {
					return (i32) PP_SET_IN_ADDRESS_INVALID;
				}
			}
		}
	}

	if (ppC->hwId == 0x8170U) {
		if ((ppInImg->width < PP_IN_MIN_WIDTH(decLinked)) ||
			(ppInImg->height < PP_IN_MIN_HEIGHT(decLinked)) ||
			(ppInImg->width > PP_IN_MAX_WIDTH(decLinked)) ||
			(ppInImg->height > PP_IN_MAX_HEIGHT(decLinked)) ||
			(ppInImg->width & PP_IN_DIVISIBILITY(decLinked)) ||
			(ppInImg->height & PP_IN_DIVISIBILITY(decLinked))) {
			return (i32) PP_SET_IN_SIZE_INVALID;
		}
	} else {
		/* Max size for pipelined pic, 511MB for JPEG etc, 1024MB for WEBP */
		u32 maxMbs =
			((decType == PP_PIPELINED_DEC_TYPE_WEBP) ||
			 ((decType == PP_PIPELINED_DEC_TYPE_JPEG) && ppC->jpeg16kSupport)) ?
			1024U : 511U;
		if ((ppInImg->width < PP_IN_MIN_WIDTH(decLinked)) ||
			(ppInImg->height < PP_IN_MIN_HEIGHT(decLinked)) ||
			(ppInImg->width > PP_IN_MAX_WIDTH_EXT(decLinked, maxMbs)) ||
			(ppInImg->height > PP_IN_MAX_HEIGHT_EXT(decLinked, maxMbs)) ||
			(ppInImg->width & PP_IN_DIVISIBILITY(decLinked)) ||
			(ppInImg->height & PP_IN_DIVISIBILITY(decLinked))) {
			return (i32) PP_SET_IN_SIZE_INVALID;
		}
	}

	if (ppInImg->picStruct > PP_PIC_BOT_FIELD_FRAME) {
		return (i32) PP_SET_IN_STRUCT_INVALID;
	} else if (ppInImg->picStruct != PP_PIC_FRAME_OR_TOP_FIELD &&
			   ppInImg->pixFormat != PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR &&
			   ppInImg->pixFormat != PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED &&
			   ppInImg->pixFormat != PP_PIX_FMT_YCRYCB_4_2_2_INTERLEAVED &&
			   ppInImg->pixFormat != PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED &&
			   ppInImg->pixFormat != PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED) {
		return (i32) PP_SET_IN_STRUCT_INVALID;
	}

	/* cropping check */
	if (ppInCrop->enable != 0) {
		if ((ppInCrop->width < PP_IN_MIN_WIDTH(0)) ||
			(ppInCrop->height < PP_IN_MIN_HEIGHT(0)) ||
			(ppInCrop->width > ppInImg->width) ||
			(ppInCrop->originX > ppInImg->width) ||
			(ppInCrop->height > ppInImg->height) ||
			(ppInCrop->originY > ppInImg->height) ||
			(ppInCrop->width & 0x07) ||
			(ppInCrop->height & 0x07) ||
			(ppInCrop->originX & 0x0F) || (ppInCrop->originY & 0x0F)) {
			return (i32) PP_SET_CROP_INVALID;
		}
#if 0
		/* when deinterlacing the cropped size has to be 16 multiple */
		if (ppCfg->ppOutDeinterlace.enable &&
			((ppInCrop->width & 0x0F) || (ppInCrop->height & 0x0F))) {
			return (i32) PP_SET_CROP_INVALID;
		}
#endif
	}
	/* check rotation */
	switch (ppInRotation->rotation) {
	case PP_ROTATION_NONE:
	case PP_ROTATION_RIGHT_90:
	case PP_ROTATION_LEFT_90:
	case PP_ROTATION_HOR_FLIP:
	case PP_ROTATION_VER_FLIP:
	case PP_ROTATION_180:
		break;
	default:
		return (i32) PP_SET_ROTATION_INVALID;
	}

	/* jpeg dec linked, rotation not supported in 440, 422, 411 and 444 */
	if (decLinked != 0 && ppInRotation->rotation != PP_ROTATION_NONE &&
		(ppInImg->pixFormat == PP_PIX_FMT_YCBCR_4_4_0 ||
		 ppInImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR ||
		 ppInImg->pixFormat == PP_PIX_FMT_YCBCR_4_1_1_SEMIPLANAR ||
		 ppInImg->pixFormat == PP_PIX_FMT_YCBCR_4_4_4_SEMIPLANAR)) {
		return (i32) PP_SET_ROTATION_INVALID;
	}

	/* rotation not supported in jpeg 400 but supported in h264 */
	if (decLinked != 0 && decType == PP_PIPELINED_DEC_TYPE_JPEG &&
		ppInImg->pixFormat == PP_PIX_FMT_YCBCR_4_0_0 &&
		ppInRotation->rotation != PP_ROTATION_NONE) {
		return (i32) PP_SET_ROTATION_INVALID;
	}

	if (ppInImg->videoRange > 1) {
		return (i32) PP_SET_IN_FORMAT_INVALID;
	}

	/* PPOutImage check */

	if (!PPIsOutPixFmtOk(ppOutImg->pixFormat, ppC)) {
		return (i32) PP_SET_OUT_FORMAT_INVALID;
	}

	if (!ppC->multiBuffer) {
		if (ppOutImg->bufferBusAddr == 0 || ppOutImg->bufferBusAddr & address_mask) {
			return (i32) PP_SET_OUT_ADDRESS_INVALID;
		}

		if (ppOutImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR) {
			if (ppOutImg->bufferChromaBusAddr == 0 ||
				(ppOutImg->bufferChromaBusAddr & address_mask)) {
				return (i32) PP_SET_OUT_ADDRESS_INVALID;
			}
		}
	}

	if (ppOutImg->width < PP_OUT_MIN_WIDTH ||
		ppOutImg->height < PP_OUT_MIN_HEIGHT ||
		ppOutImg->width > ppC->maxOutWidth ||
		ppOutImg->height > ppC->maxOutHeight) {
		return (i32) PP_SET_OUT_SIZE_INVALID;
	}

	/* scale check */
	{
		u32 w, h, multires = 0;

		w = ppInCrop->enable ? ppInCrop->width : ppInImg->width;
		h = ppInCrop->enable ? ppInCrop->height : ppInImg->height;

		if (decType == PP_PIPELINED_DEC_TYPE_VC1) {
			multires = ppInImg->vc1MultiResEnable ? 1 : 0;
		}

		/* swap width and height if input is rotated first */
		if (ppInRotation->rotation == PP_ROTATION_LEFT_90 ||
			ppInRotation->rotation == PP_ROTATION_RIGHT_90) {
			u32 tmp = w;

			w = h;
			h = tmp;
		}

		if (!ppC->scalingEna) {
			if ((w != ppOutImg->width) || (h != ppOutImg->height)) {
				return (i32) PP_SET_SCALING_UNSUPPORTED;
			}
		}

		if ((ppOutImg->width > w) &&
			(ppOutImg->width > PP_OUT_MAX_WIDTH_UPSCALED(w, multires))) {
			return (i32) PP_SET_OUT_SIZE_INVALID;
		}

		if (multires && ppOutImg->width != w) {
			return (i32) PP_SET_OUT_SIZE_INVALID;
		}

		if ((ppOutImg->height > h) &&
			(ppOutImg->height > PP_OUT_MAX_HEIGHT_UPSCALED(h, multires))) {
			return (i32) PP_SET_OUT_SIZE_INVALID;
		}

		/* Enforce downscaling factor limitation */
		if (h > ppOutImg->height ||
			w > ppOutImg->width) {
			u32 tmp;

			tmp = ppOutImg->height * PP_OUT_MIN_DOWNSCALING_FACTOR;
			if (multires) {
				tmp >>= 1;
			}
			if (h > tmp) {
				return (i32) PP_SET_OUT_SIZE_INVALID;
			}

			tmp = ppOutImg->width * PP_OUT_MIN_DOWNSCALING_FACTOR;
			if (multires) {
				tmp >>= 1;
			}
			if (w > tmp) {
				return (i32) PP_SET_OUT_SIZE_INVALID;
			}
		}

		if (multires && ppOutImg->height != h) {
			return (i32) PP_SET_OUT_SIZE_INVALID;
		}

		if (((ppOutImg->width > w) && (ppOutImg->height < h)) ||
			((ppOutImg->width < w) && (ppOutImg->height > h))) {
			return (i32) PP_SET_OUT_SIZE_INVALID;
		}
	}

	/* PPOutFrameBuffer */
	if (ppOutFrmBuffer->enable) {
		if ((ppOutFrmBuffer->frameBufferWidth > PP_MAX_FRM_BUFF_WIDTH) ||
			(ppOutFrmBuffer->writeOriginX >=
			 (i32) ppOutFrmBuffer->frameBufferWidth) ||
			(ppOutFrmBuffer->writeOriginY >=
			 (i32) ppOutFrmBuffer->frameBufferHeight) ||
			(ppOutFrmBuffer->writeOriginX + (i32) ppOutImg->width <= 0) ||
			(ppOutFrmBuffer->writeOriginY + (i32) ppOutImg->height <= 0)) {
			return (i32) PP_SET_FRAMEBUFFER_INVALID;
		}
		/* Divisibility */
		if ((ppOutFrmBuffer->writeOriginY & 1) &&
			(ppOutImg->pixFormat & PP_PIXEL_FORMAT_YUV420_MASK)) {
			return (i32) PP_SET_FRAMEBUFFER_INVALID;
		}

		if ((ppOutFrmBuffer->frameBufferHeight & 1) &&
			(ppOutImg->pixFormat & PP_PIXEL_FORMAT_YUV420_MASK)) {
			return (i32) PP_SET_FRAMEBUFFER_INVALID;
		}
	}

	/* 4x4 tiled output */
	if (ppOutImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_2_TILED_4X4 ||
		ppOutImg->pixFormat == PP_PIX_FMT_YCRYCB_4_2_2_TILED_4X4 ||
		ppOutImg->pixFormat == PP_PIX_FMT_CBYCRY_4_2_2_TILED_4X4 ||
		ppOutImg->pixFormat == PP_PIX_FMT_CRYCBY_4_2_2_TILED_4X4) {
		i32 ret = PPCheckTiledOutput(ppCfg);
		if (ret != (i32) PP_OK) {
			return ret;
		}
	}

	/* PPOutRgb */

	if ((ppOutImg->pixFormat & PP_PIXEL_FORMAT_RGB_MASK)) {
		/* Check support in HW */
		if (!ppC->ditherEna && ppOutRgb->ditheringEnable) {
			return (i32) PP_SET_DITHERING_UNSUPPORTED;
		}

		if ((ppOutRgb->rgbTransform != PP_YCBCR2RGB_TRANSFORM_CUSTOM) &&
			(ppOutRgb->rgbTransform != PP_YCBCR2RGB_TRANSFORM_BT_601) &&
			(ppOutRgb->rgbTransform != PP_YCBCR2RGB_TRANSFORM_BT_709)) {
			return (i32) PP_SET_VIDEO_ADJUST_INVALID;
		}

		if (ppOutRgb->brightness < -128 || ppOutRgb->brightness > 127) {
			return (i32) PP_SET_VIDEO_ADJUST_INVALID;
		}

		if (ppOutRgb->saturation < -64 || ppOutRgb->saturation > 128) {
			return (i32) PP_SET_VIDEO_ADJUST_INVALID;
		}

		if (ppOutRgb->contrast < -64 || ppOutRgb->contrast > 64) {
			return (i32) PP_SET_VIDEO_ADJUST_INVALID;
		}

		if ((ppOutImg->pixFormat & PP_PIXEL_FORMAT_RGB32_MASK)) {
			if (ppOutRgb->alpha > 255) {
				return (i32) PP_SET_VIDEO_ADJUST_INVALID;
			}
		} else /* 16 bits RGB */ if (ppOutRgb->transparency > 1) {
			return (i32) PP_SET_VIDEO_ADJUST_INVALID;
		}

		if (ppOutImg->pixFormat == PP_PIX_FMT_RGB32_CUSTOM) {
			PPRgbBitmask *rgbbm = &ppOutRgb->rgbBitmask;

			if ((rgbbm->maskR & rgbbm->maskG & rgbbm->maskB & rgbbm->
				 maskAlpha) != 0) {
				return (i32) PP_SET_RGB_BITMASK_INVALID;
			}
		} else if (ppOutImg->pixFormat == PP_PIX_FMT_RGB16_CUSTOM) {
			PPRgbBitmask *rgbbm = &ppOutRgb->rgbBitmask;

			if ((rgbbm->maskR & rgbbm->maskG & rgbbm->maskB & rgbbm->
				 maskAlpha) != 0 ||
				(rgbbm->maskR | rgbbm->maskG | rgbbm->maskB | rgbbm->
				 maskAlpha) >= (1 << 16)) {
				return (i32) PP_SET_RGB_BITMASK_INVALID;
			}
		}
		if ((ppOutImg->pixFormat == PP_PIX_FMT_RGB16_CUSTOM) ||
			(ppOutImg->pixFormat == PP_PIX_FMT_RGB32_CUSTOM)) {

			PPRgbBitmask *rgbbm = &ppOutRgb->rgbBitmask;

			if (PPCheckOverlapping(rgbbm->maskR,
								   rgbbm->maskG, rgbbm->maskB, rgbbm->maskAlpha)) {
				return (i32) PP_SET_RGB_BITMASK_INVALID;
			}

			if (PPContinuousCheck(rgbbm->maskR) ||
				PPContinuousCheck(rgbbm->maskG) ||
				PPContinuousCheck(rgbbm->maskB) ||
				PPContinuousCheck(rgbbm->maskAlpha)) {
				return (i32) PP_SET_RGB_BITMASK_INVALID;
			}

		}

	}

	if (ppOutMask1->enable && ppOutMask1->alphaBlendEna) {
		if (ppOutMask1->blendComponentBase & address_mask ||
			ppOutMask1->blendComponentBase == 0) {
			return (i32) PP_SET_MASK1_INVALID;
		}
	}

	if (ppOutMask2->enable && ppOutMask2->alphaBlendEna) {
		if (ppOutMask2->blendComponentBase & address_mask ||
			ppOutMask2->blendComponentBase == 0) {
			return (i32) PP_SET_MASK2_INVALID;
		}

	}

	{
		i32 ret = PPCheckAllWidthParams(ppCfg, ppC->blendEna,
										ppC->pixAccSupport,
										ppC->blendCropSupport);

		if (ret != (i32) PP_OK) {
			return ret;
		}
	}
	{
		i32 ret = PPCheckAllHeightParams(ppCfg, ppC->pixAccSupport);

		if (ret != (i32) PP_OK) {
			return ret;
		}
	}

	/* deinterlacing only for semiplanar & planar 4:2:0 */
	if (ppOutDeint->enable) {
		if (!ppC->deintEna) {
			return (i32) PP_SET_DEINTERLACING_UNSUPPORTED;
		}

		if (ppInImg->pixFormat != PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR &&
			ppInImg->pixFormat != PP_PIX_FMT_YCBCR_4_2_0_PLANAR &&
			ppInImg->pixFormat != PP_PIX_FMT_YCBCR_4_0_0) {
			return (i32) PP_SET_DEINTERLACE_INVALID;
		}
	}

	if (ppInImg->vc1RangeRedFrm &&
		(ppInImg->vc1RangeMapYEnable || ppInImg->vc1RangeMapCEnable)) {
		return (i32) PP_SET_IN_RANGE_MAP_INVALID;
	} else if (ppInImg->vc1RangeMapYCoeff > 7 || ppInImg->vc1RangeMapCCoeff > 7) {
		return (i32) PP_SET_IN_RANGE_MAP_INVALID;
	}

	return 0;
}

/*------------------------------------------------------------------------------
    Function name   : PPRun
    Description     :
    Return type     : pp result
    Argument        : PPContainer * ppC
------------------------------------------------------------------------------*/
PPResult PPRun(PPContainer *ppC)
{
	u32 bPpHwReserved = 0;

	PPSetStatus(ppC, PP_STATUS_RUNNING);

	PPDEBUG_PRINT(("pp status 2%x\n", PPGetStatus(ppC)));

	if (ppC->pipeline) {
		/* decoder has reserved the PP hardware for pipelined operation */
		ASSERT(ppC->ppCfg.ppInRotation.rotation == PP_ROTATION_NONE);
		/* Disable rotation for pipeline mode */
		ppC->ppCfg.ppInRotation.rotation = PP_ROTATION_NONE;
		SetPpRegister(ppC->ppRegs, HWIF_ROTATION_MODE, 0);

		if (ppC->decType == PP_PIPELINED_DEC_TYPE_H264 ||
			ppC->decType == PP_PIPELINED_DEC_TYPE_VP8) {
			/* h264/vp8 decoder use DLWReserveHwPipe, which reserves both DEC
			 * and PP hardware for pipeline operation.
			 * Decoder also takes care of releasing PP hardware.
			 */
			bPpHwReserved = 1;
		}
	}

	if (!bPpHwReserved) {
		if (DWLReserveHw(ppC->dwl, &ppC->coreID) != DWL_OK) {
			return PP_BUSY;
		}
	} else {
		ppC->coreID = 0; /* we just assume single core for PP */
	}

	PPFlushRegs(ppC);

	if (!ppC->pipeline) {
		/* turn ASIC ON by setting high the enable bit */
		SetPpRegister(ppC->ppRegs, HWIF_PP_E, 1);
		DWLEnableHW(ppC->dwl, ppC->coreID, PP_X170_REG_START, ppC->ppRegs[0]);
	} else {
		/* decoder turns PP ON in pipeline mode (leave enable bit low) */
		SetPpRegister(ppC->ppRegs, HWIF_PP_E, 0);
		DWLEnableHW(ppC->dwl, ppC->coreID, PP_X170_REG_START, ppC->ppRegs[0]);
	}

	return PP_OK;
}

/*------------------------------------------------------------------------------
    Function name   : PPSetFrmBufferWriting
    Description     :
    Return type     : void
    Argument        : PPContainer * ppC
------------------------------------------------------------------------------*/
void PPSetFrmBufferWriting(PPContainer *ppC)
{
	PPOutImage *ppOutImg;
	PPOutFrameBuffer *ppOutFrmBuffer;
	u32 *ppRegs;

	ASSERT(ppC != NULL);

	ppOutImg = &ppC->ppCfg.ppOutImg;
	ppOutFrmBuffer = &ppC->ppCfg.ppOutFrmBuffer;

	ppRegs = ppC->ppRegs;

	if (ppOutFrmBuffer->enable) {

		i32 up, down, right, left, scanline;

		up = ppOutFrmBuffer->writeOriginY;
		left = ppOutFrmBuffer->writeOriginX;
		down =
			((i32) ppOutFrmBuffer->frameBufferHeight - up) -
			(i32) ppOutImg->height;
		right =
			((i32) ppOutFrmBuffer->frameBufferWidth - left) -
			(i32) ppOutImg->width;

		scanline = (i32) ppOutFrmBuffer->frameBufferWidth;

		if (left < 0) {
			SetPpRegister(ppRegs, HWIF_LEFT_CROSS, (u32)(-left));
			SetPpRegister(ppRegs, HWIF_LEFT_CROSS_EXT, (u32)(-left) >> 11);
			SetPpRegister(ppRegs, HWIF_LEFT_CROSS_E, 1);
		} else {
			SetPpRegister(ppRegs, HWIF_LEFT_CROSS_E, 0);
		}
		if (right < 0) {
			SetPpRegister(ppRegs, HWIF_RIGHT_CROSS, (u32)(-right));
			SetPpRegister(ppRegs, HWIF_RIGHT_CROSS_EXT, (u32)(-right) >> 11);
			SetPpRegister(ppRegs, HWIF_RIGHT_CROSS_E, 1);
		} else {
			SetPpRegister(ppRegs, HWIF_RIGHT_CROSS_E, 0);
		}

		if (up < 0) {
			SetPpRegister(ppRegs, HWIF_UP_CROSS, (u32)(-up));
			SetPpRegister(ppRegs, HWIF_UP_CROSS_EXT, (u32)(-up) >> 11);
			SetPpRegister(ppRegs, HWIF_UP_CROSS_E, 1);
		} else {
			SetPpRegister(ppRegs, HWIF_UP_CROSS_E, 0);
		}

		if (down < 0) {
			SetPpRegister(ppRegs, HWIF_DOWN_CROSS, (u32)(-down));
			SetPpRegister(ppRegs, HWIF_DOWN_CROSS_EXT, (u32)(-down) >> 11);
			SetPpRegister(ppRegs, HWIF_DOWN_CROSS_E, 1);
		} else {
			SetPpRegister(ppRegs, HWIF_DOWN_CROSS_E, 0);
		}

		SetPpRegister(ppRegs, HWIF_DISPLAY_WIDTH,
					  ppOutFrmBuffer->frameBufferWidth);

		if (ppOutImg->pixFormat & PP_PIXEL_FORMAT_RGB_MASK) {
			ppC->frmBufferLumaOrRgbOffset =
				(scanline * up + left) * ((i32) ppC->rgbDepth / 8);
		} else if (ppOutImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED ||
				   ppOutImg->pixFormat == PP_PIX_FMT_YCRYCB_4_2_2_INTERLEAVED ||
				   ppOutImg->pixFormat == PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED ||
				   ppOutImg->pixFormat == PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED)

		{
			ppC->frmBufferLumaOrRgbOffset = (scanline * up + left) * 2;
		} else { /* PP_PIX_FMT_YCBCR_4_2_0_CH_INTERLEAVED */
			ppC->frmBufferLumaOrRgbOffset = (scanline * up + left);

			ppC->frmBufferChromaOffset = (scanline * up) / 2 + left;
		}

	} else {
		SetPpRegister(ppRegs, HWIF_DOWN_CROSS_E, 0);
		SetPpRegister(ppRegs, HWIF_LEFT_CROSS_E, 0);
		SetPpRegister(ppRegs, HWIF_RIGHT_CROSS_E, 0);
		SetPpRegister(ppRegs, HWIF_UP_CROSS_E, 0);

		SetPpRegister(ppRegs, HWIF_DISPLAY_WIDTH, ppOutImg->width);

		ppC->frmBufferLumaOrRgbOffset = 0;
		ppC->frmBufferChromaOffset = 0;
	}
}

/*------------------------------------------------------------------------------
    Function name   : PPSetRgbBitmaskCustom
    Description     :
    Return type     : void
    Argument        : PPContainer * ppC
    Argument        : u32 rgb16
------------------------------------------------------------------------------*/
void PPSetRgbBitmaskCustom(PPContainer *ppC, u32 rgb16)
{
	u32 *ppRegs;
	u32 mask, pad, alpha;
	PPRgbBitmask *rgbMask;

	ASSERT(ppC != NULL);
	rgbMask = &ppC->ppCfg.ppOutRgb.rgbBitmask;
	ppRegs = ppC->ppRegs;

	alpha = rgbMask->maskAlpha;

	if (rgb16) {
		alpha |= alpha << 16;
	}

	/* setup R */
	mask = rgbMask->maskR;

	if (rgb16) {
		mask |= mask << 16; /* duplicate mask for 16 bits RGB */
	}

	pad = PPFindFirstNonZeroBit(mask);
	SetPpRegister(ppRegs, HWIF_RGB_R_PADD, pad);
	SetPpRegister(ppRegs, HWIF_R_MASK, mask | alpha);

	/* setup G */
	mask = rgbMask->maskG;

	if (rgb16) {
		mask |= mask << 16; /* duplicate mask for 16 bits RGB */
	}

	pad = PPFindFirstNonZeroBit(mask);
	SetPpRegister(ppRegs, HWIF_RGB_G_PADD, pad);
	SetPpRegister(ppRegs, HWIF_G_MASK, mask | alpha);

	/* setup B */
	mask = rgbMask->maskB;

	if (rgb16) {
		mask |= mask << 16; /* duplicate mask for 16 bits RGB */
	}

	pad = PPFindFirstNonZeroBit(mask);
	SetPpRegister(ppRegs, HWIF_RGB_B_PADD, pad);
	SetPpRegister(ppRegs, HWIF_B_MASK, mask | alpha);
}

/*------------------------------------------------------------------------------
    Function name   : PPSetRgbBitmask
    Description     :
    Return type     : void
    Argument        : PPContainer * ppC
------------------------------------------------------------------------------*/
void PPSetRgbBitmask(PPContainer *ppC)
{
	PPOutImage *ppOutImg;
	u32 *ppRegs;

	ASSERT(ppC != NULL);

	ppOutImg = &ppC->ppCfg.ppOutImg;
	ppRegs = ppC->ppRegs;

	switch (ppOutImg->pixFormat) {
	case PP_PIX_FMT_BGR32:
		SetPpRegister(ppRegs, HWIF_B_MASK,
					  0x00FF0000 | (ppC->ppCfg.ppOutRgb.alpha << 24));
		SetPpRegister(ppRegs, HWIF_G_MASK,
					  0x0000FF00 | (ppC->ppCfg.ppOutRgb.alpha << 24));
		SetPpRegister(ppRegs, HWIF_R_MASK,
					  0x000000FF | (ppC->ppCfg.ppOutRgb.alpha << 24));
		SetPpRegister(ppRegs, HWIF_RGB_B_PADD, 8);
		SetPpRegister(ppRegs, HWIF_RGB_G_PADD, 16);
		SetPpRegister(ppRegs, HWIF_RGB_R_PADD, 24);
		break;
	case PP_PIX_FMT_RGB32:
		SetPpRegister(ppRegs, HWIF_R_MASK,
					  0x00FF0000 | (ppC->ppCfg.ppOutRgb.alpha << 24));
		SetPpRegister(ppRegs, HWIF_G_MASK,
					  0x0000FF00 | (ppC->ppCfg.ppOutRgb.alpha << 24));
		SetPpRegister(ppRegs, HWIF_B_MASK,
					  0x000000FF | (ppC->ppCfg.ppOutRgb.alpha << 24));
		SetPpRegister(ppRegs, HWIF_RGB_R_PADD, 8);
		SetPpRegister(ppRegs, HWIF_RGB_G_PADD, 16);
		SetPpRegister(ppRegs, HWIF_RGB_B_PADD, 24);
		break;

	case PP_PIX_FMT_RGB16_5_5_5: {
		u32 mask;

		mask = 0x7C00 | (ppC->ppCfg.ppOutRgb.transparency << 15);
		SetPpRegister(ppRegs, HWIF_R_MASK, mask | (mask << 16));
		mask = 0x03E0 | (ppC->ppCfg.ppOutRgb.transparency << 15);
		SetPpRegister(ppRegs, HWIF_G_MASK, mask | (mask << 16));
		mask = 0x001F | (ppC->ppCfg.ppOutRgb.transparency << 15);
		SetPpRegister(ppRegs, HWIF_B_MASK, mask | (mask << 16));
		SetPpRegister(ppRegs, HWIF_RGB_R_PADD, 1);
		SetPpRegister(ppRegs, HWIF_RGB_G_PADD, 6);
		SetPpRegister(ppRegs, HWIF_RGB_B_PADD, 11);

	}
	break;
	case PP_PIX_FMT_BGR16_5_5_5: {
		u32 mask;

		mask = 0x7C00 | (ppC->ppCfg.ppOutRgb.transparency << 15);
		SetPpRegister(ppRegs, HWIF_B_MASK, mask | (mask << 16));
		mask = 0x03E0 | (ppC->ppCfg.ppOutRgb.transparency << 15);
		SetPpRegister(ppRegs, HWIF_G_MASK, mask | (mask << 16));
		mask = 0x001F | (ppC->ppCfg.ppOutRgb.transparency << 15);
		SetPpRegister(ppRegs, HWIF_R_MASK, mask | (mask << 16));
		SetPpRegister(ppRegs, HWIF_RGB_B_PADD, 1);
		SetPpRegister(ppRegs, HWIF_RGB_G_PADD, 6);
		SetPpRegister(ppRegs, HWIF_RGB_R_PADD, 11);

	}

	break;

	case PP_PIX_FMT_RGB16_5_6_5:
		SetPpRegister(ppRegs, HWIF_R_MASK, 0xF800F800);
		SetPpRegister(ppRegs, HWIF_G_MASK, 0x07E007E0);
		SetPpRegister(ppRegs, HWIF_B_MASK, 0x001F001F);
		SetPpRegister(ppRegs, HWIF_RGB_R_PADD, 0);
		SetPpRegister(ppRegs, HWIF_RGB_G_PADD, 5);
		SetPpRegister(ppRegs, HWIF_RGB_B_PADD, 11);
		break;

	case PP_PIX_FMT_BGR16_5_6_5:
		SetPpRegister(ppRegs, HWIF_B_MASK, 0xF800F800);
		SetPpRegister(ppRegs, HWIF_G_MASK, 0x07E007E0);
		SetPpRegister(ppRegs, HWIF_R_MASK, 0x001F001F);
		SetPpRegister(ppRegs, HWIF_RGB_B_PADD, 0);
		SetPpRegister(ppRegs, HWIF_RGB_G_PADD, 5);
		SetPpRegister(ppRegs, HWIF_RGB_R_PADD, 11);
		break;

	case PP_PIX_FMT_RGB16_CUSTOM:
		PPSetRgbBitmaskCustom(ppC, 1);
		break;

	case PP_PIX_FMT_RGB32_CUSTOM:
		PPSetRgbBitmaskCustom(ppC, 0);
		break;
	default:
		ASSERT(0);  /* should never get here */
		break;
	}
}

/*------------------------------------------------------------------------------
    Function name   : PPSetRgbTransformCoeffs
    Description     :
    Return type     : void
    Argument        : PPContainer * ppC
------------------------------------------------------------------------------*/
void PPSetRgbTransformCoeffs(PPContainer *ppC)
{
	PPOutImage *ppOutImg;
	PPInImage *ppInImg;
	PPOutRgb *ppOutRgb;
	u32 *ppRegs;

	ASSERT(ppC != NULL);

	ppOutImg = &ppC->ppCfg.ppOutImg;
	ppInImg = &ppC->ppCfg.ppInImg;
	ppOutRgb = &ppC->ppCfg.ppOutRgb;

	ppRegs = ppC->ppRegs;

	if (ppOutImg->pixFormat & PP_PIXEL_FORMAT_RGB_MASK) {
		i32 satur = 0, tmp;
		PPRgbTransform *rgbT = &ppOutRgb->rgbTransformCoeffs;

		if (ppC->rgbDepth == 32) {
			SetPpRegister(ppRegs, HWIF_RGB_PIX_IN32, 0);
		} else {
			SetPpRegister(ppRegs, HWIF_RGB_PIX_IN32, 1);
		}

		/*  Contrast */
		if (ppOutRgb->contrast != 0) {
			i32 thr1y, thr2y, off1, off2, thr1, thr2, a1, a2;

			if (ppInImg->videoRange == 0) {
				i32 tmp1, tmp2;

				/* Contrast */
				thr1 = (219 * (ppOutRgb->contrast + 128)) / 512;
				thr1y = (219 - 2 * thr1) / 2;
				thr2 = 219 - thr1;
				thr2y = 219 - thr1y;

				tmp1 = (thr1y * 256) / thr1;
				tmp2 = ((thr2y - thr1y) * 256) / (thr2 - thr1);
				off1 = ((thr1y - ((tmp2 * thr1) / 256)) * (i32) rgbT->a) / 256;
				off2 = ((thr2y - ((tmp1 * thr2) / 256)) * (i32) rgbT->a) / 256;

				tmp1 = (64 * (ppOutRgb->contrast + 128)) / 128;
				tmp2 = 256 * (128 - tmp1);
				a1 = (tmp2 + off2) / thr1;
				a2 = a1 + (256 * (off2 - 1)) / (thr2 - thr1);
			} else {
				/* Contrast */
				thr1 = (64 * (ppOutRgb->contrast + 128)) / 128;
				thr1y = 128 - thr1;
				thr2 = 256 - thr1;
				thr2y = 256 - thr1y;
				a1 = (thr1y * 256) / thr1;
				a2 = ((thr2y - thr1y) * 256) / (thr2 - thr1);
				off1 = thr1y - (a2 * thr1) / 256;
				off2 = thr2y - (a1 * thr2) / 256;
			}

			if (a1 > 1023) {
				a1 = 1023;
			} else if (a1 < 0) {
				a1 = 0;
			}

			if (a2 > 1023) {
				a2 = 1023;
			} else if (a2 < 0) {
				a2 = 0;
			}

			if (thr1 > 255) {
				thr1 = 255;
			} else if (thr1 < 0) {
				thr1 = 0;
			}

			if (thr2 > 255) {
				thr2 = 255;
			} else if (thr2 < 0) {
				thr2 = 0;
			}

			if (off1 > 511) {
				off1 = 511;
			} else if (off1 < -512) {
				off1 = -512;
			}

			if (off2 > 511) {
				off2 = 511;
			} else if (off2 < -512) {
				off2 = -512;
			}

			SetPpRegister(ppRegs, HWIF_CONTRAST_THR1, (u32) thr1);
			SetPpRegister(ppRegs, HWIF_CONTRAST_THR2, (u32) thr2);

			SetPpRegister(ppRegs, HWIF_CONTRAST_OFF1, off1);
			SetPpRegister(ppRegs, HWIF_CONTRAST_OFF2, off2);

			SetPpRegister(ppRegs, HWIF_COLOR_COEFFA1, (u32) a1);
			SetPpRegister(ppRegs, HWIF_COLOR_COEFFA2, (u32) a2);

		} else {
			SetPpRegister(ppRegs, HWIF_CONTRAST_THR1, 55);
			SetPpRegister(ppRegs, HWIF_CONTRAST_THR2, 165);

			SetPpRegister(ppRegs, HWIF_CONTRAST_OFF1, 0);
			SetPpRegister(ppRegs, HWIF_CONTRAST_OFF2, 0);

			tmp = rgbT->a;

			if (tmp > 1023) {
				tmp = 1023;
			} else if (tmp < 0) {
				tmp = 0;
			}

			SetPpRegister(ppRegs, HWIF_COLOR_COEFFA1, tmp);
			SetPpRegister(ppRegs, HWIF_COLOR_COEFFA2, tmp);
		}

		/*  brightness */
		SetPpRegister(ppRegs, HWIF_COLOR_COEFFF, ppOutRgb->brightness);

		/* saturation */
		satur = 64 + ppOutRgb->saturation;

		tmp = (satur * (i32) rgbT->b) / 64;
		if (tmp > 1023) {
			tmp = 1023;
		} else if (tmp < 0) {
			tmp = 0;
		}
		SetPpRegister(ppRegs, HWIF_COLOR_COEFFB, (u32) tmp);

		tmp = (satur * (i32) rgbT->c) / 64;
		if (tmp > 1023) {
			tmp = 1023;
		} else if (tmp < 0) {
			tmp = 0;
		}
		SetPpRegister(ppRegs, HWIF_COLOR_COEFFC, (u32) tmp);

		tmp = (satur * (i32) rgbT->d) / 64;
		if (tmp > 1023) {
			tmp = 1023;
		} else if (tmp < 0) {
			tmp = 0;
		}
		SetPpRegister(ppRegs, HWIF_COLOR_COEFFD, (u32) tmp);

		tmp = (satur * (i32) rgbT->e) / 64;
		if (tmp > 1023) {
			tmp = 1023;
		} else if (tmp < 0) {
			tmp = 0;
		}

		SetPpRegister(ppRegs, HWIF_COLOR_COEFFE, (u32) tmp);
	}
}

/*------------------------------------------------------------------------------
    Function name   : PPFindFirstNonZeroBit
    Description     :
    Return type     : u32
    Argument        : u32 mask
------------------------------------------------------------------------------*/
u32 PPFindFirstNonZeroBit(u32 mask)
{
	u32 offset = 0;

	while (!(mask & 0x80000000) && (offset < 32)) {
		mask <<= 1;
		offset++;
	}

	return offset & 0x1F;
}

/*------------------------------------------------------------------------------
    Function name   : PPIsInPixFmtOk
    Description     :
    Return type     : u32
    Argument        : u32 pix_fmt
    Argument        : const PPContainer * ppC
------------------------------------------------------------------------------*/
u32 PPIsInPixFmtOk(u32 pix_fmt, const PPContainer *ppC)
{
	u32 ret = 1;
	const i32 decLinked = ppC->decInst == NULL ? 0 : 1;

	switch (pix_fmt) {
	case PP_PIX_FMT_YCBCR_4_2_0_TILED:
		if (ppC->decType == PP_PIPELINED_DEC_TYPE_JPEG) {
			ret = 0;
		}
		break;
	case PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR:
		break;
	case PP_PIX_FMT_YCBCR_4_2_0_PLANAR:
	case PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED:
		/* these are not supported in pipeline */
		if (decLinked != 0) {
			ret = 0;
		}
		break;
	case PP_PIX_FMT_YCBCR_4_0_0:
		/* this supported just in H264 and JPEG pipeline mode */
		if (decLinked == 0 ||
			(ppC->decType != PP_PIPELINED_DEC_TYPE_JPEG &&
			 ppC->decType != PP_PIPELINED_DEC_TYPE_H264)) {
			ret = 0;
		}
		/* H264 monochrome not supported in 8170 */
		if ((ppC->hwId == 0x8170U) &&
			(ppC->decType == PP_PIPELINED_DEC_TYPE_H264)) {
			ret = 0;
		}
		break;
	case PP_PIX_FMT_YCBCR_4_2_2_SEMIPLANAR:
	case PP_PIX_FMT_YCBCR_4_4_0:
	case PP_PIX_FMT_YCBCR_4_1_1_SEMIPLANAR:
	case PP_PIX_FMT_YCBCR_4_4_4_SEMIPLANAR:
		/* these supported just in JPEG pipeline mode */
		if (decLinked == 0 || ppC->decType != PP_PIPELINED_DEC_TYPE_JPEG) {
			ret = 0;
		}
		break;
	case PP_PIX_FMT_YCRYCB_4_2_2_INTERLEAVED:
	case PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED:
	case PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED:
		/* these are not supported in pipeline and in 8170 */
		if (decLinked != 0 || (ppC->hwId == 0x8170U)) {
			ret = 0;
		}
		break;
	default:
		ret = 0;
	}

	return ret;
}

/*------------------------------------------------------------------------------
    Function name   : PPIsOutPixFmtOk
    Description     :
    Return type     : u32
    Argument        : u32 pix_fmt
    Argument        : const PPContainer * ppC
------------------------------------------------------------------------------*/
u32 PPIsOutPixFmtOk(u32 pix_fmt, const PPContainer *ppC)
{
	switch (pix_fmt) {
	case PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR:
	case PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED:
	case PP_PIX_FMT_RGB16_CUSTOM:
	case PP_PIX_FMT_RGB16_5_5_5:
	case PP_PIX_FMT_RGB16_5_6_5:
	case PP_PIX_FMT_BGR16_5_5_5:
	case PP_PIX_FMT_BGR16_5_6_5:
	case PP_PIX_FMT_RGB32_CUSTOM:
	case PP_PIX_FMT_RGB32:
	case PP_PIX_FMT_BGR32:
		return 1;
	case PP_PIX_FMT_YCRYCB_4_2_2_INTERLEAVED:
	case PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED:
	case PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED:
		if (ppC->hwId == 0x8170U) {
			return 0;
		} else {
			return 1;
		}
	case PP_PIX_FMT_YCBCR_4_2_2_TILED_4X4:
	case PP_PIX_FMT_YCRYCB_4_2_2_TILED_4X4:
	case PP_PIX_FMT_CBYCRY_4_2_2_TILED_4X4:
	case PP_PIX_FMT_CRYCBY_4_2_2_TILED_4X4:
		if (ppC->hwId == 0x8170U) {
			return 0;
		} else {
			return (ppC->tiledEna);
		}
	default:
		return 0;
	}
}

/*------------------------------------------------------------------------------
    Function name   : PPIsOutPixFmtBlendOk
    Description     :
    Return type     : u32
    Argument        : u32 pix_fmt
------------------------------------------------------------------------------*/
u32 PPIsOutPixFmtBlendOk(u32 pix_fmt)
{
	switch (pix_fmt) {
	case PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED:
	case PP_PIX_FMT_YCRYCB_4_2_2_INTERLEAVED:
	case PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED:
	case PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED:
	case PP_PIX_FMT_YCBCR_4_2_2_TILED_4X4:
	case PP_PIX_FMT_YCRYCB_4_2_2_TILED_4X4:
	case PP_PIX_FMT_CBYCRY_4_2_2_TILED_4X4:
	case PP_PIX_FMT_CRYCBY_4_2_2_TILED_4X4:
	case PP_PIX_FMT_RGB16_CUSTOM:
	case PP_PIX_FMT_RGB16_5_5_5:
	case PP_PIX_FMT_RGB16_5_6_5:
	case PP_PIX_FMT_BGR16_5_5_5:
	case PP_PIX_FMT_BGR16_5_6_5:
	case PP_PIX_FMT_RGB32_CUSTOM:
	case PP_PIX_FMT_RGB32:
	case PP_PIX_FMT_BGR32:
		return 1;
	default:
		return 0;
	}
}

/*------------------------------------------------------------------------------
    Function name   : PPSetupScaling
    Description     :
    Return type     : void
    Argument        : PPContainer * ppC
    Argument        : const PPOutImage *ppOutImg
------------------------------------------------------------------------------*/
void PPSetupScaling(PPContainer *ppC, const PPOutImage *ppOutImg)
{
	u32 *ppRegs = ppC->ppRegs;
	PPInCropping *ppInCrop;
	u32 inWidth, inHeight;
	u32 inActHeight;
	u32 outActHeight;
	u32 pixFormatOk;

	ppInCrop = &ppC->ppCfg.ppInCrop;

	if (ppInCrop->enable) {
		inActHeight = ppInCrop->height;
	} else {
		inActHeight = ppC->inHeight;
	}

	/* swap width and height if input is rotated first */
	if (ppC->ppCfg.ppInRotation.rotation == PP_ROTATION_LEFT_90 ||
		ppC->ppCfg.ppInRotation.rotation == PP_ROTATION_RIGHT_90) {
		if (ppInCrop->enable) {
			inWidth = ppInCrop->height;
			inHeight = ppInCrop->width;
		} else {
			inWidth = ppC->inHeight;
			inHeight = ppC->inWidth;
		}
		outActHeight = ppOutImg->width;
	} else {
		if (ppInCrop->enable) {
			inWidth = ppInCrop->width;
			inHeight = ppInCrop->height;
		} else {
			inWidth = ppC->inWidth;
			inHeight = ppC->inHeight;
		}
		outActHeight = ppOutImg->height;
	}

	if (inWidth < ppOutImg->width) {
		/* upscale */
		u32 W, invW;

		SetPpRegister(ppRegs, HWIF_HOR_SCALE_MODE, 1);

		W = FDIVI(TOFIX((ppOutImg->width - 1), 16), (inWidth - 1));

		SetPpRegister(ppRegs, HWIF_SCALE_WRATIO, W);

		invW = FDIVI(TOFIX((inWidth - 1), 16), (ppOutImg->width - 1));

		SetPpRegister(ppRegs, HWIF_WSCALE_INVRA, invW);
	} else if (inWidth > ppOutImg->width) {
		/* downscale */

		SetPpRegister(ppRegs, HWIF_HOR_SCALE_MODE, 2);

		ppC->cHnorm = FDIVI(TOFIX((ppOutImg->width), 16), inWidth);
		ppC->cHfast = FDIVI(TOFIX((2 * ppOutImg->width), 16), inWidth);
		ppC->cHfast4x = FDIVI(TOFIX((4 * ppOutImg->width), 16), inWidth);
		if (ppC->cHworkaroundFlag) {
			u32 pos0, pos1;
			pos0 = ppC->cHnorm * inWidth >> 16;
			pos1 = ppC->cHnorm * (inWidth - 1) >> 16;
			if (pos0 != pos1) { /* HW "leftover" bug */
				/* Increase scaling coefficient until full pixel generated
				 * from last input-pixel. */
				while (pos0 < ppOutImg->width) {
					ppC->cHnorm++;
					pos0 = ppC->cHnorm * inWidth >> 16;
				}
			}
		}

		SetPpRegister(ppRegs, HWIF_WSCALE_INVRA, ppC->cHnorm);
	} else {
		SetPpRegister(ppRegs, HWIF_WSCALE_INVRA, 0);
		SetPpRegister(ppRegs, HWIF_HOR_SCALE_MODE, 0);
	}

	if (inHeight < ppOutImg->height) {
		/* upscale */
		u32 H, invH;

		SetPpRegister(ppRegs, HWIF_VER_SCALE_MODE, 1);

		H = FDIVI(TOFIX((ppOutImg->height - 1), 16), (inHeight - 1));

		SetPpRegister(ppRegs, HWIF_SCALE_HRATIO, H);

		invH = FDIVI(TOFIX((inHeight - 1), 16), (ppOutImg->height - 1));

		SetPpRegister(ppRegs, HWIF_HSCALE_INVRA, invH);
	} else if (inHeight > ppOutImg->height) {
		/* downscale */
		u32 Cv;

		Cv = FDIVI(TOFIX((ppOutImg->height), 16), inHeight) + 1;

		SetPpRegister(ppRegs, HWIF_VER_SCALE_MODE, 2);

		SetPpRegister(ppRegs, HWIF_HSCALE_INVRA, Cv);
	} else {
		SetPpRegister(ppRegs, HWIF_HSCALE_INVRA, 0);
		SetPpRegister(ppRegs, HWIF_VER_SCALE_MODE, 0);
	}

	/* Check vertical scaling shortcut via interlacing input content */
	if (inActHeight > outActHeight &&
		!ppC->fastVerticalDownscaleDisable &&
		ppC->decInst &&
		ppC->decType != PP_PIPELINED_DEC_TYPE_JPEG &&
		ppC->decType != PP_PIPELINED_DEC_TYPE_WEBP) {
		/* downscale */

		ppC->cVnorm = FDIVI(TOFIX((outActHeight), 16), inActHeight) + 1;
		ppC->cVfast = FDIVI(TOFIX((outActHeight), 16), inActHeight / 2) + 1;
		ppC->fastScaleMode = -1;

		/* If downscaling by at least factor of two, try field processing
		 * to speed up things. Following criteria must be met:
		 *  - standalone PP
		 *  - no 8px crop
		 *  - progressive input frame */
		if (ppC->ppCfg.ppInImg.picStruct == PP_PIC_FRAME_OR_TOP_FIELD &&
			inActHeight >= outActHeight * 2 &&
			(inActHeight & 0xF) == 0) {
			ppC->fastVerticalDownscale = 1;

			/* Set scaling parameters */
			if (inActHeight == outActHeight * 2) {
				/* Scaling factor is 1/2, so just process top field */
				ppC->fastScaleMode = 0;
			} else {
				/* Scaling factor is < 1/2, so we must process fields *and*
				 * scale */
				ppC->fastScaleMode = 2;
			}
		} else { /* Regular downscale */
			ppC->fastVerticalDownscale = 0;
		}
	}

	/* Only 4:2:0 and 4:0:0 inputs supported */
	pixFormatOk =
		(ppC->ppCfg.ppInImg.pixFormat == PP_PIX_FMT_YCBCR_4_2_0_PLANAR ||
		 ppC->ppCfg.ppInImg.pixFormat == PP_PIX_FMT_YCBCR_4_2_0_SEMIPLANAR ||
		 ppC->ppCfg.ppInImg.pixFormat == PP_PIX_FMT_YCBCR_4_2_0_TILED ||
		 ppC->ppCfg.ppInImg.pixFormat == PP_PIX_FMT_YCBCR_4_0_0);

	/* If in stand-alone mode (without dec), apply fast vertical downscaling
	 * settings now */
	/* NOTE: prefer quality in stand-alone mode; this is now disabled */
#if 0
	if (ppC->fastVerticalDownscale &&
		ppC->ppCfg.ppInImg.picStruct == PP_PIC_FRAME_OR_TOP_FIELD &&
		ppC->decInst == NULL &&
		pixFormatOk) {
		u32 rotate90;

		rotate90 = (ppC->ppCfg.ppInRotation.rotation == PP_ROTATION_LEFT_90 ||
					ppC->ppCfg.ppInRotation.rotation == PP_ROTATION_RIGHT_90);

		/* Set new image height and cropping parameters */
		SetPpRegister(ppRegs, HWIF_PP_IN_STRUCT, PP_PIC_TOP_FIELD_FRAME);

		/* Set new scaling coefficient */
		if (rotate90) {
			SetPpRegister(ppRegs, HWIF_WSCALE_INVRA, ppC->cHfast);
		} else {
			SetPpRegister(ppRegs, HWIF_HSCALE_INVRA, ppC->cVfast);
		}

		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_H_EXT,
					  ((((ppC->inHeight + 31) / 32) & 0x700) >> 8));
		SetPpRegister(ppC->ppRegs, HWIF_PP_IN_HEIGHT,
					  (((ppC->inHeight + 31) / 32) & 0x0FF));
		if (inHeight & 0x1F) {
			SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_D_E, 1);
		} else {
			SetPpRegister(ppC->ppRegs, HWIF_PP_CROP8_D_E, 0);
		}

		/* Disable deinterlacing */
		SetPpRegister(ppRegs, HWIF_DEINT_E, 0);

		/* Set fast scale mode */
		if (!rotate90) {
			SetPpRegister(ppRegs, HWIF_VER_SCALE_MODE, ppC->fastScaleMode);
		}
	} else {
		/*ppC->fastVerticalDownscale = 0;*/
	}
#endif

	/* Fast horizontal downscaling allowed if
	 * 1) Downscaling ratio at least 1/2
	 * 2) No 8px crop
	 * 3) HW supports it (and not disabled explicitly)
	 * 4) Pixel format is valid (i.e. 4:2:0 or 4:0:0)
	 * 5) Using dec+pp in video mode
	 */
	ppC->fastHorizontalDownscale = 0;
	if (ppC->fastScalingSupport == PP_FAST_SCALING_SUPPORTED &&
		!ppC->fastHorizontalDownscaleDisable &&
		pixFormatOk &&
		ppOutImg->width * 2 < inWidth &&
		!(ppInCrop->width & 0x0F) &&
		ppC->decInst &&
		ppC->decType != PP_PIPELINED_DEC_TYPE_JPEG &&
		ppC->decType != PP_PIPELINED_DEC_TYPE_WEBP) {
		/* Recalculate horizontal coefficient */
		SetPpRegister(ppRegs, HWIF_PP_FAST_SCALE_E, 1);
		ppC->fastHorizontalDownscale = 1;

		/* Check special case with +-90 deg rotation */
		if (ppC->ppCfg.ppInRotation.rotation == PP_ROTATION_LEFT_90 ||
			ppC->ppCfg.ppInRotation.rotation == PP_ROTATION_RIGHT_90) {
			/* If scaled width is less than 1/4 of original, we can use
			 * both horizontal and vertical shortcuts here (i.e. use
			 * 4x scaling coefficient */
			if ((ppOutImg->width * 4 < inWidth) && ppC->fastVerticalDownscale) {
				SetPpRegister(ppRegs, HWIF_WSCALE_INVRA, ppC->cHfast4x);
			}
			/* If scaled width is >= 1/4x, we must turn off either of
			 * the optimizations, in this case we choose to turn off
			 * horizontal shortcut */
			else if (ppC->fastVerticalDownscale) {
				SetPpRegister(ppRegs, HWIF_PP_FAST_SCALE_E, 0);
				ppC->fastHorizontalDownscale = 0;
			}
			/* Vertical downscale shortcut not used, so just enable
			 * horizontal component */
			else {
				SetPpRegister(ppRegs, HWIF_WSCALE_INVRA, ppC->cHfast);
			}
		} else {
			SetPpRegister(ppRegs, HWIF_WSCALE_INVRA, ppC->cHfast);
		}
	} else {
		SetPpRegister(ppRegs, HWIF_PP_FAST_SCALE_E, 0);
	}
}

/*------------------------------------------------------------------------------
    Function name   : PPCheckAllXParams
    Description     :
    Return type     : u32
    Argument        : PPConfig * ppCfg
------------------------------------------------------------------------------*/
static i32 PPCheckAllWidthParams(PPConfig *ppCfg, u32 blendEna, u32 pixAcc,
								 u32 ablendCrop)
{
	PPOutMask1 *ppOutMask1;
	PPOutMask2 *ppOutMask2;
	PPOutImage *ppOutImg;
	PPOutFrameBuffer *ppOutFrmBuffer;
	u32 frameBufferRequired = 0; /* Flag to signal that frame buffer is required. This is the
                                  * case if out size is "pixel accurate" */

	i32 ret = (i32) PP_OK;
	u32 multiple;
	u32 multipleFrmBuffer;

	ASSERT(ppCfg != NULL);
	ppOutMask1 = &ppCfg->ppOutMask1;
	ppOutMask2 = &ppCfg->ppOutMask2;

	ppOutImg = &ppCfg->ppOutImg;
	ppOutFrmBuffer = &ppCfg->ppOutFrmBuffer;

	multipleFrmBuffer = PP_X170_DATA_BUS_WIDTH;
	if (ppOutImg->pixFormat & PP_PIXEL_FORMAT_RGB_MASK) {
		if (ppOutImg->pixFormat & PP_PIXEL_FORMAT_RGB32_MASK) {
			multipleFrmBuffer = multipleFrmBuffer / 4;    /* 4 bytes per pixel */
		} else {
			multipleFrmBuffer = multipleFrmBuffer / 2;    /* 2 bytes per pixel */
		}
	} else if (ppOutImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED ||
			   ppOutImg->pixFormat == PP_PIX_FMT_YCRYCB_4_2_2_INTERLEAVED ||
			   ppOutImg->pixFormat == PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED ||
			   ppOutImg->pixFormat == PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED) {
		multipleFrmBuffer = multipleFrmBuffer / 2;    /* 2 bytes per pixel */
	}

	if (pixAcc) {
		if ((ppOutImg->pixFormat & PP_PIXEL_FORMAT_YUV420_MASK) ||
			(ppOutImg->pixFormat & PP_PIXEL_FORMAT_YUV422_MASK)) {
			multiple = 2;
		} else { /* RGB, YUV4:4:4, YUV4:0:0 */
			multiple = 1;
		}
	} else {
		multiple = multipleFrmBuffer;
	}

	if (!pixAcc) {
		if (ppOutImg->width & (WIDTH_MULTIPLE - 1)) {
			ret = (i32) PP_SET_OUT_SIZE_INVALID;
		}
	} else {
		if (ppOutImg->width & (multiple - 1)) {
			ret = (i32) PP_SET_OUT_SIZE_INVALID;
		}
		/* Do we have "pixel accurate output width? */
		if ((ppOutImg->width & (WIDTH_MULTIPLE - 1)) && !pixAcc) {
			/* If output width is in "pixel domain", we must have a frame
			 * buffer specified. Exception to this check happens when PP
			 * output is in tiled mode; here we skip the frame buffer
			 * check for that and trust on PPCheckTiledOutput() */
			/* the tiled output format not support accurate pixel out */
			if (ppOutImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_2_TILED_4X4 &&
				ppOutImg->pixFormat == PP_PIX_FMT_YCRYCB_4_2_2_TILED_4X4 &&
				ppOutImg->pixFormat == PP_PIX_FMT_CBYCRY_4_2_2_TILED_4X4 &&
				ppOutImg->pixFormat == PP_PIX_FMT_CRYCBY_4_2_2_TILED_4X4) {
				frameBufferRequired = 1;
			}
		}
	}

	if (ppOutMask1->enable && (ppOutMask1->width & (multiple - 1))) {
		ret = (i32) PP_SET_MASK1_INVALID;
	}
	if (ppOutMask1->enable && (ppOutMask1->originX & (multiple - 1))) {
		ret = (i32) PP_SET_MASK1_INVALID;
	}
	if (ppOutMask2->enable && (ppOutMask2->width & (multiple - 1))) {
		ret = (i32) PP_SET_MASK2_INVALID;
	}
	if (ppOutMask2->enable && (ppOutMask2->originX & (multiple - 1))) {
		ret = (i32) PP_SET_MASK2_INVALID;
	}

	if (ppOutMask1->enable && (ppOutMask1->width == 0 ||
							   ppOutMask1->height == 0)) {
		ret = (i32) PP_SET_MASK1_INVALID;
	}
	if (ppOutMask2->enable && (ppOutMask2->width == 0 ||
							   ppOutMask2->height == 0)) {
		ret = (i32) PP_SET_MASK2_INVALID;
	}

	/* Check if blending is enabled in HW */
	if (ppOutMask1->alphaBlendEna && !blendEna) {
		ret = (i32) PP_SET_ABLEND_UNSUPPORTED;
	}

	if (ppOutMask2->alphaBlendEna && !blendEna) {
		ret = (i32) PP_SET_ABLEND_UNSUPPORTED;
	}

	if (ppOutMask1->enable && ppOutMask1->alphaBlendEna) {
		/* Blending masks only for 422 and rbg */
		if (!PPIsOutPixFmtBlendOk(ppOutImg->pixFormat)) {
			ret = (i32) PP_SET_OUT_FORMAT_INVALID; /*PP_SET_MASK1_INVALID;*/
		}

		if ((ppOutMask1->width + ppOutMask1->originX) > ppOutImg->width) {
			ret = (i32) PP_SET_MASK1_INVALID;
		}

		if (ppOutMask1->originY < 0) {
			ret = (i32) PP_SET_MASK1_INVALID;
		}

		if ((ppOutMask1->height + ppOutMask1->originY) > ppOutImg->height) {
			ret = (i32) PP_SET_MASK1_INVALID;
		}

		if (ppOutMask1->originX < 0) {
			ret = (i32) PP_SET_MASK1_INVALID;
		}

		/* More checks if cropped alphablending supported */
		if (ablendCrop) {
			if (ppOutMask1->blendWidth > PP_IN_MAX_BLEND_SCANLINE) {
				ret = (i32) PP_SET_MASK1_INVALID;
			}

			if (ppOutMask1->blendWidth == 0 ||
				ppOutMask1->blendHeight == 0) {
				ret = (i32) PP_SET_MASK1_INVALID;
			}

			if (ppOutMask1->originX < 0) {
				ret = (i32) PP_SET_MASK1_INVALID;
			}

			if (ppOutMask1->originY < 0) {
				ret = (i32) PP_SET_MASK1_INVALID;
			}

			/* Check blend area issues */
			if ((ppOutMask1->width + ppOutMask1->blendOriginX) > ppOutMask1->blendWidth) {
				ret = (i32) PP_SET_MASK1_INVALID;
			}

			if ((ppOutMask1->height + ppOutMask1->blendOriginY) > ppOutMask1->blendHeight) {
				ret = (i32) PP_SET_MASK1_INVALID;;
			}

			/* Blend area width must be multiple of 2 */
			if (ppOutMask1->blendWidth & 1) {
				ret = (i32) PP_SET_MASK1_INVALID;
			}
		} else {
			/* if alpha blend crop feature is not supported by HW, these
			 * must be left to zero */
			if (ppOutMask1->blendOriginX != 0 ||
				ppOutMask1->blendOriginY != 0 ||
				ppOutMask1->blendWidth != 0 ||
				ppOutMask1->blendHeight != 0) {
				ret = (i32) PP_SET_MASK1_INVALID;
			}
		}

	}

	if (ppOutMask2->enable && ppOutMask2->alphaBlendEna) {
		/* Blending masks only for 422 and rbg */
		if (!PPIsOutPixFmtBlendOk(ppOutImg->pixFormat)) {
			ret = (i32) PP_SET_OUT_FORMAT_INVALID; /*PP_SET_MASK2_INVALID;*/
		}

		if ((ppOutMask2->width + ppOutMask2->originX) > ppOutImg->width) {
			ret = (i32) PP_SET_MASK2_INVALID;
		}

		if (ppOutMask2->originY < 0) {
			ret = (i32) PP_SET_MASK2_INVALID;
		}

		if ((ppOutMask2->height + ppOutMask2->originY) > ppOutImg->height) {
			ret = (i32) PP_SET_MASK2_INVALID;
		}

		if (ppOutMask2->originX < 0) {
			ret = (i32) PP_SET_MASK2_INVALID;
		}

		/* More checks if cropped alphablending supported */
		if (ablendCrop) {
			if (ppOutMask2->blendWidth > PP_IN_MAX_BLEND_SCANLINE) {
				ret = (i32) PP_SET_MASK2_INVALID;
			}

			if (ppOutMask2->blendWidth == 0 ||
				ppOutMask2->blendHeight == 0) {
				ret = (i32) PP_SET_MASK2_INVALID;
			}

			if (ppOutMask2->originX < 0) {
				ret = (i32) PP_SET_MASK2_INVALID;
			}

			if (ppOutMask2->originY < 0) {
				ret = (i32) PP_SET_MASK2_INVALID;
			}

			/* Check blend area issues */
			if ((ppOutMask2->width + ppOutMask2->blendOriginX) > ppOutMask2->blendWidth) {
				ret = (i32) PP_SET_MASK2_INVALID;
			};

			if ((ppOutMask2->height + ppOutMask2->blendOriginY) > ppOutMask2->blendHeight) {
				ret = (i32) PP_SET_MASK2_INVALID;
			};

			/* Blend area width must be multiple of 2 */
			if (ppOutMask2->blendWidth & 1) {
				ret = (i32) PP_SET_MASK2_INVALID;
			}
		} else {
			/* if alpha blend crop feature is not supported by HW, these
			 * must be left to zero */
			if (ppOutMask2->blendOriginX != 0 ||
				ppOutMask2->blendOriginY != 0 ||
				ppOutMask2->blendWidth != 0 ||
				ppOutMask2->blendHeight != 0) {
				ret = (i32) PP_SET_MASK2_INVALID;
			}
		}

	}

	if (ppOutFrmBuffer->enable) {
		i32 tmp;

		if ((ppOutFrmBuffer->frameBufferWidth & (multipleFrmBuffer - 1))) {
			ret = (i32) PP_SET_FRAMEBUFFER_INVALID;
		}
		if ((ppOutFrmBuffer->writeOriginX & (multiple - 1))) {
			ret = (i32) PP_SET_FRAMEBUFFER_INVALID;
		}

		tmp = ppOutFrmBuffer->writeOriginX;
		tmp = tmp > 0 ? tmp : (-1) * tmp;

		if (((u32) tmp & (multiple - 1))) {
			ret = (i32) PP_SET_FRAMEBUFFER_INVALID;
		}
	} else if (frameBufferRequired) {
		ret = (i32) PP_SET_FRAMEBUFFER_INVALID;
	}

	return ret;
}

/*------------------------------------------------------------------------------
    Function name   : PPCheckAllHeightParams
    Description     :
    Return type     : u32
    Argument        : PPConfig * ppCfg
------------------------------------------------------------------------------*/
i32 PPCheckAllHeightParams(PPConfig *ppCfg, u32 pixAcc)
{
	PPOutMask1 *ppOutMask1;
	PPOutMask2 *ppOutMask2;
	PPOutImage *ppOutImg;

	i32 ret = (i32) PP_OK;
	/*u32 multiple;*/

	ASSERT(ppCfg != NULL);

	ppOutMask1 = &ppCfg->ppOutMask1;
	ppOutMask2 = &ppCfg->ppOutMask2;

	ppOutImg = &ppCfg->ppOutImg;

#if 0
	multiple = PP_X170_DATA_BUS_WIDTH;

	if (ppOutImg->pixFormat & PP_PIXEL_FORMAT_RGB_MASK) {
		if (ppOutImg->pixFormat & PP_PIXEL_FORMAT_RGB32_MASK) {
			multiple = multiple / 4;    /* 4 bytes per pixel */
		} else {
			multiple = multiple / 2;    /* 2 bytes per pixel */
		}
	} else if (ppOutImg->pixFormat == PP_PIX_FMT_YCBCR_4_2_2_INTERLEAVED ||
			   ppOutImg->pixFormat == PP_PIX_FMT_YCRYCB_4_2_2_INTERLEAVED ||
			   ppOutImg->pixFormat == PP_PIX_FMT_CBYCRY_4_2_2_INTERLEAVED ||
			   ppOutImg->pixFormat == PP_PIX_FMT_CRYCBY_4_2_2_INTERLEAVED) {
		multiple = multiple / 2;    /* 2 bytes per pixel */
	}
#endif


	if (!pixAcc) {
		if (ppOutImg->height & (HEIGHT_MULTIPLE - 1)) {
			return (i32) PP_SET_OUT_SIZE_INVALID;
		}
	} else {
		u32 multiple = 0;
		/* pixel accurate output supported, figure out minimum granularity */
		if (ppOutImg->pixFormat & PP_PIXEL_FORMAT_YUV420_MASK) {
			multiple = 2;
		} else { /* RGB, YUV4:4:4, YUV4:2:2, YUV4:0:0 */
			multiple = 1;
		}
		if (ppOutImg->height & (multiple - 1)) {
			ret = (i32) PP_SET_OUT_SIZE_INVALID;
		}
	}


	if (ppOutMask1->enable &&
		(ppOutImg->pixFormat & PP_PIXEL_FORMAT_YUV420_MASK) &&
		(ppOutMask1->originY & 1)) {
		ret = (i32) PP_SET_MASK1_INVALID;
	}

	if (ppOutMask1->enable &&
		(ppOutImg->pixFormat & PP_PIXEL_FORMAT_YUV420_MASK) &&
		(ppOutMask1->height & 1)) {
		ret = (i32) PP_SET_MASK1_INVALID;
	}

	if (ppOutMask2->enable &&
		(ppOutImg->pixFormat & PP_PIXEL_FORMAT_YUV420_MASK) &&
		(ppOutMask2->originY & 1)) {
		ret = (i32) PP_SET_MASK2_INVALID;
	}

	if (ppOutMask2->enable &&
		(ppOutImg->pixFormat & PP_PIXEL_FORMAT_YUV420_MASK) &&
		(ppOutMask2->height & 1)) {
		ret = (i32) PP_SET_MASK2_INVALID;
	}

	return ret;
}

/*------------------------------------------------------------------------------
    Function name   : PPContinuosCheck
    Description     : Check are the ones only one-after-another
    Return type     : i32
    Argument        : u32
------------------------------------------------------------------------------*/
i32 PPContinuousCheck(u32 value)
{

	i32 ret = (i32) PP_OK;
	u32 first = 0;
	u32 tmp = 0;

	if (value) {
		do {
			tmp = value & 1;
			if (tmp) {
				ret = (i32) PP_OK;
			} else {
				ret = (i32) PP_PARAM_ERROR;
			}

			first |= tmp;

			value = value >> 1;
			if (!tmp && !tmp && first) {
				break;
			}

		} while (value);
	}

	return ret;
}

/*------------------------------------------------------------------------------
    Function name   : PPCheckOverlapping
    Description     : Check if values overlap
    Return type     : i32
    Argument        : u32 a b c d
------------------------------------------------------------------------------*/

i32 PPCheckOverlapping(u32 a, u32 b, u32 c, u32 d)
{

	if ((a & b) || (a & c) || (a & d) || (b & c) || (b & d) || (c & d)) {
		return (i32) PP_PARAM_ERROR;
	} else {
		return (i32) PP_OK;
	}

}

/*------------------------------------------------------------------------------
    Function name   : PPSelectOutputSize
    Description     : Select output size  based on the HW version info
    Return type     : i32
    Argument        : pp container *
------------------------------------------------------------------------------*/

i32 PPSelectOutputSize(PPContainer *ppC)
{
	u32 tmp;
	DWLHwConfig_t hwConfig;

	ASSERT(ppC != NULL);

	ppC->altRegs = 1;

	DWLReadAsicConfig(&hwConfig);
	ppC->maxOutWidth = hwConfig.maxPpOutPicWidth;
	ppC->maxOutHeight = 4096;

	ppC->blendEna = hwConfig.ppConfig & PP_ALPHA_BLENDING ? 1 : 0;
	ppC->deintEna = hwConfig.ppConfig & PP_DEINTERLACING ? 1 : 0;
	ppC->ditherEna = hwConfig.ppConfig & PP_DITHERING ? 1 : 0;
	ppC->scalingEna = hwConfig.ppConfig & PP_SCALING ? 1 : 0;
	ppC->tiledEna = hwConfig.ppConfig & PP_TILED_4X4 ? 1 : 0;
	ppC->pixAccSupport = hwConfig.ppConfig & PP_PIX_ACC_OUTPUT ? 1 : 0;
	ppC->blendCropSupport = hwConfig.ppConfig & PP_ABLEND_CROP ? 1 : 0;

	if (ppC->fastScalingSupport == PP_FAST_SCALING_UNINITIALIZED) {
#define PP_SCALING_BITS_OFFSET (26)

		/* scale_level needs to be 0x3 in order to support "fast" downscaling */
		tmp = (hwConfig.ppConfig & PP_SCALING) >> PP_SCALING_BITS_OFFSET;
		if (tmp == 0x3) {
			ppC->fastScalingSupport = PP_FAST_SCALING_SUPPORTED;
		} else {
			ppC->fastScalingSupport = PP_FAST_SCALING_NOT_SUPPORTED;
		}
	}

	PPDEBUG_PRINT(("Alt regs, output size %d\n", ppC->maxOutHeight));
	return (i32) PP_OK;
}

/*------------------------------------------------------------------------------
    Function name   : PPSetDithering
    Description     : Set up dithering
    Return type     :
    Argument        : pp container *
------------------------------------------------------------------------------*/
static void PPSetDithering(PPContainer *ppC)
{
	PPOutImage *ppOutImg;
	PPRgbBitmask *rgbbm;
	u32 *ppRegs;
	u32 tmp = 0;

	ASSERT(ppC != NULL);

	ppOutImg = &ppC->ppCfg.ppOutImg;
	ppRegs = ppC->ppRegs;

	switch (ppOutImg->pixFormat) {
	case PP_PIX_FMT_RGB16_5_5_5:
	case PP_PIX_FMT_BGR16_5_5_5:
		SetPpRegister(ppRegs, HWIF_DITHER_SELECT_R, 2);
		SetPpRegister(ppRegs, HWIF_DITHER_SELECT_G, 2);
		SetPpRegister(ppRegs, HWIF_DITHER_SELECT_B, 2);
		break;
	case PP_PIX_FMT_RGB16_5_6_5:
	case PP_PIX_FMT_BGR16_5_6_5:
		SetPpRegister(ppRegs, HWIF_DITHER_SELECT_R, 2);
		SetPpRegister(ppRegs, HWIF_DITHER_SELECT_G, 3);
		SetPpRegister(ppRegs, HWIF_DITHER_SELECT_B, 2);
		break;
	case PP_PIX_FMT_RGB16_CUSTOM:
	case PP_PIX_FMT_RGB32_CUSTOM:

		rgbbm = &ppC->ppCfg.ppOutRgb.rgbBitmask;

		tmp = PPSelectDitheringValue(rgbbm->maskR);
		SetPpRegister(ppRegs, HWIF_DITHER_SELECT_R, tmp);

		tmp = PPSelectDitheringValue(rgbbm->maskG);
		SetPpRegister(ppRegs, HWIF_DITHER_SELECT_G, tmp);

		tmp = PPSelectDitheringValue(rgbbm->maskB);
		SetPpRegister(ppRegs, HWIF_DITHER_SELECT_B, tmp);

		break;

	default:
		break;
	}

}

/*------------------------------------------------------------------------------
    Function name   : PPCountOnes
    Description     : one ones in value
    Return type     : number of ones
    Argument        : u32 value
------------------------------------------------------------------------------*/
static u32 PPCountOnes(u32 value)
{
	u32 ones = 0;

	if (value) {
		do {
			if (value & 1) {
				ones++;
			}

			value = value >> 1;
		} while (value);
	}
	return ones;
}

/*------------------------------------------------------------------------------
    Function name   : PPSelectDitheringValue
    Description     : select dithering matrix for color depth set in mask
    Return type     : u32, dithering value which is set to HW
    Argument        : u32 mask, mask for selecting color depth
------------------------------------------------------------------------------*/
static u32 PPSelectDitheringValue(u32 mask)
{

	u32 ones = 0;
	u32 ret = 0;

	ones = PPCountOnes(mask);

	switch (ones) {
	case 4:
		ret = 1;
		break;
	case 5:
		ret = 2;
		break;
	case 6:
		ret = 3;
		break;
	default:
		ret = 0;
		break;
	}

	return ret;

}

/*------------------------------------------------------------------------------
    Function name   : WaitForPp
    Description     : Wait PP HW to finish
    Return type     : PPResult
    Argument        : PPContainer *
------------------------------------------------------------------------------*/
PPResult WaitForPp(PPContainer *ppC)
{
	const void *dwl;
	i32 dwlret = 0;
	u32 irq_stat;
	PPResult ret = PP_OK;

	dwl = ppC->dwl;

	dwlret = DWLWaitHwReady(dwl, ppC->coreID, (u32)(-1));

	if (dwlret == DWL_HW_WAIT_TIMEOUT) {
		ret = PP_HW_TIMEOUT;
	} else if (dwlret == DWL_HW_WAIT_ERROR) {
		ret = PP_SYSTEM_ERROR;
	}

	PPRefreshRegs(ppC);

	irq_stat = GetPpRegister(ppC->ppRegs, HWIF_PP_IRQ_STAT);

	/* make sure ASIC is OFF */
	SetPpRegister(ppC->ppRegs, HWIF_PP_E, 0);
	SetPpRegister(ppC->ppRegs, HWIF_PP_IRQ, 0);
	SetPpRegister(ppC->ppRegs, HWIF_PP_IRQ_STAT, 0);
	/* also disable pipeline bit! */
	SetPpRegister(ppC->ppRegs, HWIF_PP_PIPELINE_E, 0);

	DWLDisableHW(ppC->dwl, ppC->coreID, PP_X170_REG_START, ppC->ppRegs[0]);
	DWLReleaseHw(ppC->dwl, ppC->coreID);

	PPSetStatus(ppC, PP_STATUS_IDLE);

	if (irq_stat & DEC_8170_IRQ_BUS) {
		ret = PP_HW_BUS_ERROR;
	}

	return ret;
}

/*------------------------------------------------------------------------------
    Function name   : PPCheckSetupChanges
    Description     : Check changes in PP config
    Return type     : PPResult
    Argument        : PPContainer *
------------------------------------------------------------------------------*/
u32 PPCheckSetupChanges(PPConfig *prevCfg, PPConfig *newCfg)
{

	u32 changes = 0;

	PPOutImage *prevOutImg, *newOutImg;
	PPInCropping *prevInCropping, *newInCropping;
	PPOutMask1 *prevMask1, *newMask1;
	PPOutMask2 *prevMask2, *newMask2;
	PPOutFrameBuffer *prevFrameBuffer, *newFrameBuffer;
	PPInRotation *prevRotation, *newRotation;

	prevOutImg = &prevCfg->ppOutImg;
	prevInCropping = &prevCfg->ppInCrop;
	prevMask1 = &prevCfg->ppOutMask1;
	prevMask2 = &prevCfg->ppOutMask2;
	prevFrameBuffer = &prevCfg->ppOutFrmBuffer;
	prevRotation = &prevCfg->ppInRotation;

	newOutImg = &newCfg->ppOutImg;
	newInCropping = &newCfg->ppInCrop;
	newMask1 = &newCfg->ppOutMask1;
	newMask2 = &newCfg->ppOutMask2;
	newFrameBuffer = &newCfg->ppOutFrmBuffer;
	newRotation = &newCfg->ppInRotation;

	/* output picture parameters */
	if (prevOutImg->width != newOutImg->width ||
		prevOutImg->height != newOutImg->height ||
		prevOutImg->pixFormat != newOutImg->pixFormat) {
		changes++;
	}

	/* checked bacause changes pipeline status */
	if (prevInCropping->enable != newInCropping->enable) {
		changes++;
	}

	/* checked bacause changes pipeline status */
	if (prevRotation->rotation != newRotation->rotation) {
		changes++;
	}

	if (prevMask1->enable != newMask1->enable ||
		prevMask1->originX != newMask1->originX ||
		prevMask1->originY != newMask1->originY ||
		prevMask1->height != newMask1->height ||
		prevMask1->width != newMask1->width) {
		changes++;
	}

	if (prevMask2->enable != newMask2->enable ||
		prevMask2->originX != newMask2->originX ||
		prevMask2->originY != newMask2->originY ||
		prevMask2->height != newMask2->height ||
		prevMask2->width != newMask2->width) {
		changes++;
	}

	if (prevFrameBuffer->enable != newFrameBuffer->enable ||
		prevFrameBuffer->writeOriginX != newFrameBuffer->writeOriginX ||
		prevFrameBuffer->writeOriginY != newFrameBuffer->writeOriginY ||
		prevFrameBuffer->frameBufferWidth != newFrameBuffer->frameBufferWidth ||
		prevFrameBuffer->frameBufferHeight != newFrameBuffer->frameBufferHeight) {
		changes++;
	}

	return changes;

}

